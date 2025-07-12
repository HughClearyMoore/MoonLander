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

static void UnlinkECSSystems(Game* game)
{
	PhysicsSystemDestroy(game);
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

ECS* GameECS(Game* game)
{
	return &game->ecs;
}

void GameStart(Game* game)
{
	srand(time(NULL));

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

	PhysicsSystem* physics = &game->ecs.systems.physics;
	RenderingSystem* rendering = &game->ecs.systems.rendering;
	ScriptSystem* scripts = &game->ecs.systems.scripts;

	Transform t = TransformIdentity();
	t.position.x = 0.5f;
	t.position.y = 0.5f;
	t.position.z = 0.1f;
	
	

	Entity_t entity = MLECSNewEntity(&game->ecs);
	Entity_t entity_2 = MLECSNewEntity(&game->ecs);

	MLECSAttachComponentTransform(&game->ecs, entity, &t);

	t.position.x = 3;
	{
		vec3 axis = { 1.0f, 0.0f, 0.0f };
		TransformRotate(&t, axis, 45.0f);
	}
	

	MLECSAttachComponentTransform(&game->ecs, entity_2, &t);


	Mesh* entry = MLMeshManagerGetMesh(&game->mesh_manager, "elephant");
	ShaderProgram* program = MLShaderProgramManagerGetProgram(&game->program_manager, "basic");	

	Mesh* teapot = MLMeshManagerGetMesh(&game->mesh_manager, "teapot");	

	Transform* t_ptr = MLECSGetComponentTransform(&game->ecs, entity);

	Model mod_component = CreateModel(entry, program);
	Model teapot_model = CreateModel(teapot, program);

	MLECSAttachComponentModel(&game->ecs, entity_2, &mod_component);
	MLECSAttachComponentModel(&game->ecs, entity, &teapot_model);

	MLECSRemoveComponentTransform(&game->ecs, entity);

	t.rotation.x = 0.0;
	t.rotation.y = 0.0;
	t.rotation.z = 0.0;
	t.rotation.w = 1.0;

	t.position.x = 0.1;

	t.scale.scale_x = 0.5;
	t.scale.scale_y = 0.5;
	t.scale.scale_z = 0.5;

	MLECSAttachComponentTransform(&game->ecs, entity, &t);

	Name name_component = NameComponentCreate("teapot");
	MLECSAttachComponentName(&game->ecs, entity, &name_component);

	Script teapot_script = ScriptComponentCreate(game, SCRIPT_ENUM_TeapotScript);
	MLECSAttachComponentScript(&game->ecs, entity, &teapot_script);

	{
		dMass m;
		dMassSetBox(&m, 1.0, 1.0, 1.0, 1.0);
		RigidBody rb = RigidBodyCreate(&game->ecs, entity_2, physics->world, &m);		

		MLECSAttachComponentRigidBody(&game->ecs, entity_2, &rb);

		RigidBodyAddTorque(&game->ecs, entity_2, (vec3) { 0.0f, 1.0f, 0.0f }, 1);

		dGeomID box = dCreateBox(0, 1.0, 1.0, 1.0);
		Collider collider = ColliderDynamicCreate(game, entity_2, box, 0.01, 0.2, 0.05);

		MLECSAttachComponentCollider(GameECS(game), entity_2, &collider);

		Script elephant_script = ScriptComponentCreate(game, SCRIPT_ENUM_ElephantScript);

		MLECSAttachComponentScript(&game->ecs, entity_2, &elephant_script);
	}
	
	Entity_t elephant_child = MLECSNewEntity(&game->ecs);
	
	{
		Mesh* cube_mesh = MLMeshManagerGetMesh(&game->mesh_manager, "cube");

		Model child_model = CreateModel(cube_mesh, program);		

		MLECSAttachComponentModel(&game->ecs, elephant_child, &child_model);

		Transform trans = TransformIdentity();
		trans.position.x = -1.0;
		trans.position.y = 1.0;
		trans.position.z = -1.0;
		

		MLECSAttachComponentTransform(&game->ecs, elephant_child, &trans);

		Parent p = ParentCreate(&game->ecs, entity_2);
		MLECSAttachComponentParent(&game->ecs, elephant_child, &p);
	}

	Entity_t child_child = MLECSNewEntity(&game->ecs);

	{
		Mesh* cube_mesh = MLMeshManagerGetMesh(&game->mesh_manager, "cube");

		Model child_model = CreateModel(cube_mesh, program);		

		MLECSAttachComponentModel(&game->ecs, child_child, &child_model);

		Transform trans = TransformIdentity();
		trans.position.x = -1.0;
		trans.position.y = -1.0;
		trans.position.z = -1.0;

		MLECSAttachComponentTransform(&game->ecs, child_child, &trans);

		Parent p = ParentCreate(&game->ecs, elephant_child);
		MLECSAttachComponentParent(&game->ecs, child_child, &p);
	}

	Entity_t terrain = MLECSNewEntity(&game->ecs);

	{
		Mesh* plane_mesh = MLMeshManagerGetMesh(&game->mesh_manager, "plane");
		Model plane_model = CreateModel(plane_mesh, program);

		MLECSAttachComponentModel(&game->ecs, terrain, &plane_model);

		Transform trans = TransformIdentity();

		trans.position.y = -5.0;

		trans.scale.scale_x = 10.0;
		trans.scale.scale_z = 10.0;

		MLECSAttachComponentTransform(&game->ecs, terrain, &trans);

		dSpaceID space = PhysicsSystemCurrentSpace(game);

		dGeomID shape = dCreatePlane(0, 0.0, 1.0, 0.0, -5.0);

		Collider collider = ColliderStaticCreate(game,			
			terrain,
			shape,
			0.01,
			1.0,
			0.05);

		MLECSAttachComponentCollider(&game->ecs, terrain, &collider);		
	}

	//
	const double physics_frequency = 60;
	const double fixed_dt = 1 / physics_frequency;

	double time = glfwGetTime();
	double accumulator = 0.0;
	

	//PhysicsSystemUpdate(game, physics, fixed_dt);

	while (!glfwWindowShouldClose(game->window) && game->is_running)
	{
		double now = glfwGetTime();
		double frame_time = now - time;
		time = now;

		accumulator += frame_time;

		glfwPollEvents();
		MLECSReadyMarkedEntities(game);

		ScriptSystemUpdate(game, scripts, frame_time);

		if (accumulator >= fixed_dt)
		{
			PhysicsSystemUpdate(game, physics, fixed_dt);
			accumulator -= fixed_dt;
		}		

		
		MLECSDestroyMarkedEntities(game);
		MLInputResetMarked(&game->input);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderingSystemUpdate(game, rendering, frame_time, accumulator / fixed_dt);
		glfwSwapBuffers(game->window);
	}

	UnlinkECSSystems(game);
}