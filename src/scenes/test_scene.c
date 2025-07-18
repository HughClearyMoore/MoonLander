#include "scenes/test_scene.h"

#include "MLScene.h"

#include "MLCore.h"

SCENE_DEFAULT_CREATE_FUNCTION(Test, TestSceneCtx)

SCENE_READY_FUNCTION(Test)
{
	ECS* ecs = GameECS(game);	

	Transform transform = TransformIdentity();

	Model model = CreateModel3DHelper(game, "cube", "basic", NULL);

	Entity_t entity = MLECSNewEntity(ecs);
	MLECSAttachComponentTransform(ecs, entity, &transform);
	MLECSAttachComponentModel(ecs, entity, &model);


	Transform camera_transform = TransformIdentity();
	Entity_t camera = GameCreateCamera(game, &camera_transform);

	Script player_script = ScriptComponentCreate(game, SCRIPT_ENUM_PlayerScript);

	MLECSAttachComponentScript(ecs, camera, &player_script);



	Entity_t teapot = MLECSNewEntity(ecs);

	Model teapot_model = CreateModel3DHelper(game, "teapot", "basic", NULL);

	MLECSAttachComponentModel(ecs, teapot, &teapot_model);

	Transform teapot_transform = TransformIdentity();

	teapot_transform.position.x += 5.0;

	MLECSAttachComponentTransform(ecs, teapot, &teapot_transform);

	Name teapot_name = NameComponentCreate("teapot");

	MLECSAttachComponentName(ecs, teapot, &teapot_name);

	Script teapot_script = ScriptComponentCreate(game, SCRIPT_ENUM_TeapotScript);

	MLECSAttachComponentScript(ecs, teapot, &teapot_script);

}

SCENE_UPDATE_FUNCTION(Test)
{
	Input* input = GameInput(game);

	if (input->keys[GLFW_KEY_ENTER].just_pressed)
	{
		SceneManager* sm = GameSceneManager(game);

		SceneManagerSwitch(sm, game, "Main");

	}

}

SCENE_DESTROY_FUNCTION(Test)
{

}

SCENE_ENTER_FUNCTION(Test)
{

}

SCENE_EXIT_FUNCTION(Test)
{

}