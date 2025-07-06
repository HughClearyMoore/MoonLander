#include "MLCore.h"

#include <assert.h>
#include <string.h>

#include "MLColours.h"

#include "MLShader.h"
#include "MLMesh.h"
#include "MLTypes.h"
#include "MLModel.h"
#include "MLAssets.h"
#include "MLScript.h"

#include <stdio.h>

#include <ode/ode.h>

#include "ECS/Component.h"

#include "systems/PhysicsSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ScriptSystem.h"

#define DRAW_DEBUG 0

static void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

static void LinkECSSystems(Game* game)
{
	game->ecs.systems.rendering = RenderingSystemCreate(game);
	game->ecs.systems.physics = PhysicsSystemCreate(game);
	game->ecs.systems.scripts = ScriptSystemCreate(game);
	game->ecs.systems.names = NameSystemCreate(game);
}

Game GameCreate(const size_t width, const size_t height, const char* title)
{
	Game game;
	memset(&game, 0, sizeof(game));

	{
		int res = glfwInit();
		assert(res == GLFW_TRUE && "Failed to init GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	game.window_width = width;
	game.window_height = height;
	game.title = STIStringCreate(title);
	
	game.window = glfwCreateWindow(width, height, game.title.data, NULL, NULL);
	assert(game.window && "Failed to create game window");

	glfwMakeContextCurrent(game.window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		assert(0 && "Failed to init GLAD");
	}


	glViewport(0, 0, width, height);	

	glfwSetFramebufferSizeCallback(game.window, &WindowResizeCallback);
	glfwSetKeyCallback(game.window, (GLFWkeyfun)&MLInputKeyCallback);

	



	game.mesh_manager = MLMeshManagerCreate();
	game.program_manager = MLShaderProgramManagerCreate();
	game.script_manager = MLScriptManagerCreate();

	game.ecs = MLECSCreate();

	MLLoadAssets(&game);

	return game;
}

void GameDestroy(Game* game)
{
	MLECSDestroy(&game->ecs);
	MLScriptManagerDestroy(&game->script_manager);
	MLShaderProgramManagerDestroy(&game->program_manager);
	MLMeshManagerDestroy(&game->mesh_manager);

	glfwDestroyWindow(game->window);
	STIStringDestroy(&game->title);


	glfwTerminate();
}

void GameStart(Game* game)
{
	LinkECSSystems(game);


	RenderingSystemInitialise(&game->ecs.systems.rendering, game);

	game->is_running = STI_TRUE;

	glClearColor(
		ML_Colour_TardisBlue.r,
		ML_Colour_TardisBlue.g,
		ML_Colour_TardisBlue.b,
		1.0f);

	glEnable(GL_DEPTH_TEST);

#if DRAW_DEBUG

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0);

#endif

	// ODE test

	dInitODE2(0);

	dWorldID world = dWorldCreate();

	dBodyID body = dBodyCreate(world);
	dBodySetPosition(body, 0.0, 1.0, 0.0);

	const dReal* pos = dBodyGetPosition(body);
	printf("Body position: x=%.2f, y=%.2f, z=%.2f\n", pos[0], pos[1], pos[2]);

	dBodyDestroy(body);
	dWorldDestroy(world);
	dCloseODE();

	//

	PhysicsSystem* physics = &game->ecs.systems.physics;
	RenderingSystem* rendering = &game->ecs.systems.rendering;
	ScriptSystem* scripts = &game->ecs.systems.scripts;


	Transform t = { .x = 0.5f, .y = 0.5f, .z = 0.1f, .scale = 1.0f };

	Entity_t entity = MLECSNewEntity(&game->ecs);
	Entity_t entity_2 = MLECSNewEntity(&game->ecs);

	MLECSAttachComponentTransform(&game->ecs, entity, &t);

	t.x = 0.2;

	MLECSAttachComponentTransform(&game->ecs, entity_2, &t);


	Mesh* entry = MLMeshManagerGetMesh(&game->mesh_manager, "elephant");
	ShaderProgram* program = MLShaderProgramManagerGetProgram(&game->program_manager, "basic");	

	Mesh* teapot = MLMeshManagerGetMesh(&game->mesh_manager, "teapot");	

	Transform* t_ptr = MLECSGetComponentTransform(&game->ecs, entity);

	Model mod_component = { .mesh = entry, .program = program };
	Model teapot_model = { .mesh = teapot, .program = program };

	MLECSAttachComponentModel(&game->ecs, entity_2, &mod_component);
	MLECSAttachComponentModel(&game->ecs, entity, &teapot_model);

	MLECSRemoveComponentTransform(&game->ecs, entity);

	t.x = 0.1;
	t.scale = 0.5;

	MLECSAttachComponentTransform(&game->ecs, entity, &t);

	Name name_component = NameComponentCreate("teapot");
	MLECSAttachComponentName(&game->ecs, entity, &name_component);

	Script teapot_script = ScriptComponentCreate(game, SCRIPT_ENUM_TeapotScript);
	MLECSAttachComponentScript(&game->ecs, entity, &teapot_script);

	//Script script = ScriptComponentCreate(game, SCRIPT_ENUM_PlayerScript);

	//MLECSAttachComponentScript(&game->ecs, entity_2, &script);

	//script.script->ready(game, script.context); // this needs to be called by some sort of scene management		

	double time = glfwGetTime();

	while (!glfwWindowShouldClose(game->window) && game->is_running)
	{
		double now = glfwGetTime();
		double delta_time = now - time;
		time = now;

		glfwPollEvents();

		MLECSReadyMarkedEntities(game);

		ScriptSystemUpdate(game, scripts, delta_time);

		MLECSDestroyMarkedEntities(game);

		MLInputResetMarked(&game->input);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		RenderingSystemUpdate(game, rendering, delta_time);

		glfwSwapBuffers(game->window);
	}	
}