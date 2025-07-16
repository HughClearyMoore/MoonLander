#include "scenes/main_scene.h"

#include <stdio.h>

#include "MLScene.h"

#include "MLCore.h"

SCENE_DEFAULT_CREATE_FUNCTION(Main, MainSceneContext)

SCENE_READY_FUNCTION(Main)
{
	ECS* ecs = GameECS(game);

	Entity_t teapot = MLECSNewEntity(ecs);

	Transform t = TransformIdentity();

	MLECSAttachComponentTransform(ecs, teapot, &t);

	Mesh* teapot_mesh = MLMeshManagerGetMesh(&game->mesh_manager, "teapot");
	ShaderProgram* program = MLShaderProgramManagerGetProgram(&game->program_manager, "basic");

	Model teapot_model = CreateModel3D(teapot_mesh, program, NULL);

	MLECSAttachComponentModel(ecs, teapot, &teapot_model);

	Name teapot_name = NameComponentCreate("teapot");

	MLECSAttachComponentName(ecs, teapot, &teapot_name);

	Script teapot_script = ScriptComponentCreate(game, SCRIPT_ENUM_TeapotScript);

	MLECSAttachComponentScript(ecs, teapot, &teapot_script);


	Entity_t terrain = MLECSNewEntity(ecs);

	Model plane_model = CreateModel3DHelper(game, "plane", "model_3d", "moon_texture");

	MLECSAttachComponentModel(ecs, terrain, &plane_model);

	Transform plane_transform = TransformIdentity();

	plane_transform.position.y = -5.0;

	plane_transform.scale.scale_x = 10.0;
	plane_transform.scale.scale_z = 10.0;

	MLECSAttachComponentTransform(ecs, terrain, &plane_transform);

	dSpaceID space = PhysicsSystemCurrentSpace(game);

	printf("Space for plane: %zu\n", (size_t)space);

	dGeomID shape = dCreatePlane(0, 0.0, 1.0, 0.0, -5.0);

	Collider collider = ColliderStaticCreate(
		game,
		terrain,
		shape,
		0.01,
		1.0,
		0.05
	);

	MLECSAttachComponentCollider(ecs, terrain, &collider);

	Transform camera_transform = TransformIdentity();

	Entity_t camera = GameCreateCamera(game, &camera_transform);

	Script camera_script = ScriptComponentCreate(game, SCRIPT_ENUM_PlayerScript);

	MLECSAttachComponentScript(ecs, camera, &camera_script);

}

SCENE_UPDATE_FUNCTION(Main)
{
	MainSceneContext* main_scene = (MainSceneContext*)context;
	Input* input = GameInput(game);

	main_scene->x += 1;

	if (main_scene->x >= 100)
	{
		//printf("updating!\n");
		main_scene->x = 0;
	}



	if (input->keys[GLFW_KEY_ENTER].just_pressed)
	{
		SceneManager* sm = GameSceneManager(game);

		SceneManagerSwitch(sm, game, "Test");

	}
}

SCENE_DESTROY_FUNCTION(Main)
{

}

SCENE_ENTER_FUNCTION(Main)
{
	printf("main scene entered!\n");
}

SCENE_EXIT_FUNCTION(Main)
{
	printf("main scene exited!\n");
}