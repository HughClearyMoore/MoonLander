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

#define DRAW_DEBUG 0

static void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
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

	PhysicsSystem physics = PhysicsSystemCreate(&game->ecs);

	Transform t = { .x = 0.5f, .y = 0.5f, .z = 0.1f, .scale = 1.0f };

	Entity_t entity = MLECSNewEntity(&game->ecs);
	Entity_t entity_2 = MLECSNewEntity(&game->ecs);

	MLECSAttachComponentTransform(&game->ecs, entity, &t);

	t.x = 50.0;

	MLECSAttachComponentTransform(&game->ecs, entity_2, &t);

	
	Transform* t_ptr = MLECSGetComponentTransform(&game->ecs, entity);


	// surely fucking not?

	PhysicsSystemUpdate(&physics, 1.0);


	Script* scr = MLScriptManagetGet(&game->script_manager, SCRIPT_ENUM_PlayerScript);

	scr->create(game, &scr->ctx);
	scr->ready(game, scr->ctx);

	Mesh* entry = MLMeshManagerGetMesh(&game->mesh_manager, "elephant");
	ShaderProgram* program = MLShaderProgramManagerGetProgram(&game->program_manager, "basic");

	Model model = MLModelCreate(entry, program);

	double time = glfwGetTime();

	while (!glfwWindowShouldClose(game->window) && game->is_running)
	{
		double now = glfwGetTime();
		double delta_time = now - time;
		time = now;

		glfwPollEvents();

		scr->update(game, scr->ctx, delta_time);

		MLInputResetMarked(&game->input);



		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		MLModelDraw(&model, delta_time);

		glfwSwapBuffers(game->window);
	}

	scr->destroy(game, scr->ctx);
}