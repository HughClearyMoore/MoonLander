#include "MLCore.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <ode/ode.h>

#include "MLColours.h"

#include "MLShader.h"
#include "MLMesh.h"
#include "MLTypes.h"
#include "MLModel.h"
#include "MLAssets.h"
#include "MLScript.h"



#include "ECS/Component.h"

#include "systems/PhysicsSystem.h"
#include "systems/RenderingSystem.h"
#include "systems/ScriptSystem.h"

#include "MLSceneManager.h"

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
	game.managers.scene_manager = SceneManagerCreate();
	game.managers.texture_manager = MLTextureManagerCreate();

	MLLoadAssets(&game);
	
	dInitODE2(0);

	return game;
}

void GameDestroy(Game* game)
{
	SceneManagerDestroy(game, &game->managers.scene_manager);

	dCloseODE();

	MLScriptManagerDestroy(&game->script_manager);
	MLShaderProgramManagerDestroy(&game->program_manager);
	MLMeshManagerDestroy(&game->mesh_manager);
	MLTextureManagerDestroy(&game->managers.texture_manager);

	glfwDestroyWindow(game->window);
	STIStringDestroy(&game->title);


	glfwTerminate();
}

ECS* GameECS(Game* game)
{
	return game->managers.scene_manager.current->ecs;	
}

Input* GameInput(Game* game)
{
	return &game->input;
}

void GameStart(Game* game)
{
	srand(time(NULL));

	//RenderingSystemInitialise(&game->ecs.systems.rendering, game);

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

	//
	SceneManagerSetScene(GameSceneManager(game), game, "Main");	
	//

	const double physics_frequency = 60;
	const double fixed_dt = 1 / physics_frequency;

	double time = glfwGetTime();
	double accumulator = 0.0;
	

	//PhysicsSystemUpdate(game, physics, fixed_dt);

	while (!glfwWindowShouldClose(game->window) && game->is_running)
	{
		ECS* ecs = GameECS(game);
		double now = glfwGetTime();
		double frame_time = now - time;
		time = now;

		accumulator += frame_time;

		glfwPollEvents();
		MLECSReadyMarkedEntities(game);

		
		ecs = GameECS(game);

		ScriptSystemUpdate(game, &ecs->systems.scripts, frame_time);

		if (accumulator >= fixed_dt)
		{
			PhysicsSystemUpdate(game, &ecs->systems.physics, fixed_dt);
			accumulator -= fixed_dt;
		}		
		
		MLECSDestroyMarkedEntities(game);				

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderingSystemUpdate(game, &ecs->systems.rendering, frame_time, accumulator / fixed_dt);
		glfwSwapBuffers(game->window);

		SceneManagerUpdate(&game->managers.scene_manager, game, frame_time);

		MLInputResetMarked(&game->input);
	}
}

Entity_t GameCreateCamera(Game* game, Transform* transform)
{
	ECS* ecs = GameECS(game);
	Entity_t camera = MLECSNewEntity(ecs);

	MLECSAttachComponentTransform(ecs, camera, transform);

	RenderingSystem* rs = &ecs->systems.rendering;

	rs->camera = camera;

	return camera;
}