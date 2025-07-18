#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "MLSceneManager.h"
#include "MLCore.h"

static void RegisterSceneFactories(HashMap* map)
{
	Scene scene = { 0 };

#define SCENE(scene_name) \
	scene.name = STIStringCreate(#scene_name); \
	scene.callbacks.create = &SceneCreate##scene_name; \
	scene.callbacks.ready = &SceneReady##scene_name; \
	scene.callbacks.update = &SceneUpdate##scene_name; \
	scene.callbacks.destroy = &SceneDestroy##scene_name; \
	scene.callbacks.enter = &SceneEnter##scene_name; \
	scene.callbacks.exit = &SceneExit##scene_name; \
	HashMapInsert(map, scene.name.data, &scene); \
	scene = (Scene){ 0 };

#include "../defs/MLScenes.defs"

#undef SCENE	
}


static void SceneDeleter(void* scene_ptr)
{
	Scene* scene = *(Scene**)scene_ptr;

	MLECSDestroy(scene->ecs);

	STIStringDestroy(&scene->name);
	free(scene->context);

	free(scene);
}

static void FactoryDeleter(void* ptr)
{
	Scene* scene = (Scene*)ptr;

	STIStringDestroy(&scene->name);
}

SceneManager SceneManagerCreate()
{
	SceneManager manager = { 0 };
	
	manager.scene_factory = HashMapCreate(
		HASHMAP_KEY_STRING,
		sizeof(Scene),
		32,
		&HashMapFNV1AHash,
		&HashMapStringCmp,
		&FactoryDeleter
	);	

	manager.live_scenes = DynArrayCreate(sizeof(Scene*), 0, &SceneDeleter);

	RegisterSceneFactories(&manager.scene_factory);

	return manager;
}

void SceneManagerDestroy(SceneManager* scene_manager)
{	
	HashMapDestroy(&scene_manager->scene_factory);
	DynArrayDestroy(&scene_manager->live_scenes);
}

void InitialiseECSSystems(Game* game, ECS* ecs)
{
	ecs->systems.rendering = RenderingSystemCreate(game, ecs);
	ecs->systems.physics = PhysicsSystemCreate(game, ecs);
	ecs->systems.scripts = ScriptSystemCreate(game, ecs);
	ecs->systems.names = NameSystemCreate(game, ecs);
}

void UninitialiseECSSystems(Game* game, ECS* ecs)
{
	PhysicsSystemDestroy(game, ecs);
}

Scene* SceneManagerCreateScene(SceneManager* scene_manager, Game* game, const char* name)
{
	Scene* scene = (Scene*)HashMapGet(&scene_manager->scene_factory, name);
	assert(scene);
	
	Scene* new_scene = malloc(sizeof(Scene));

	assert(new_scene);
	
	*new_scene = *scene;
	new_scene->name = STIStringClone(&scene->name);
	new_scene->is_ready = STI_FALSE;

	// initialise ECS etc.

	new_scene->ecs = malloc(sizeof(ECS));

	assert(new_scene->ecs);

	*new_scene->ecs = MLECSCreate();

	assert(new_scene->ecs);	
	
	InitialiseECSSystems(game, new_scene->ecs);

	new_scene->callbacks.create(game, &new_scene->context);

	DynArrayPush(&scene_manager->live_scenes, &new_scene);

	return new_scene;
}

void SceneManagerDestroyScene(SceneManager* scene_manager, Game* game, Scene* scene)
{
	scene->callbacks.exit(game, scene->context);
	scene->callbacks.destroy(game, scene->context);

	UninitialiseECSSystems(game, scene->ecs);
	MLECSDestroy(scene->ecs);

	const size_t sz = DynArraySize(&scene_manager->live_scenes);
	Scene** scene_ptr = scene_manager->live_scenes.data;

	for (size_t i = 0; i < sz; ++i)
	{
		Scene* scn = scene_ptr[i];

		if (scn == scene)
		{
			// swap and pop
			const size_t back_index = sz - 1;

			DynArraySwap(&scene_manager->live_scenes, i, back_index, Scene*);
			DynArrayPopBack(&scene_manager->live_scenes);
			break;
		}
	}

	free(scene->ecs);
}

void SceneManagerSwitch(SceneManager* scene_manager, Game* game, const char* name)
{
	const size_t sz = DynArraySize(&scene_manager->live_scenes);
	Scene** live_scenes = scene_manager->live_scenes.data;

	for (size_t i = 0; i < sz; ++i)
	{
		Scene* scene = live_scenes[i];

		if (strcmp(scene->name.data, name) == 0)
		{
			scene_manager->next = scene;
			return;
		}
	}

	Scene* new_scene = SceneManagerCreateScene(scene_manager, game, name);

	scene_manager->next = new_scene;
}

void SceneManagerSetScene(SceneManager* scene_manager, Game* game, const char* name)
{
	const size_t sz = DynArraySize(&scene_manager->live_scenes);

	Scene** live_scenes = scene_manager->live_scenes.data;

	for (size_t i = 0; i < sz; ++i)
	{
		Scene* scene = live_scenes[i];

		if (strcmp(scene->name.data, name) == 0)
		{
			scene_manager->current = scene;
			return;
		}
	}

	Scene* new_scene = SceneManagerCreateScene(scene_manager, game, name);

	scene_manager->current = new_scene;
	scene_manager->current->callbacks.ready(game, scene_manager->current->context);
	scene_manager->current->is_ready = STI_TRUE;
	scene_manager->current->callbacks.enter(game, scene_manager->current->context);
}

void SceneManagerSwitchAndDestroy(SceneManager* scene_manager, Game* game, const char* name)
{
	Scene* old = scene_manager->current;

	SceneManagerSwitch(scene_manager, game, name);

	if (old)
	{
		SceneManagerDestroyScene(scene_manager, game, old);
	}
}

SceneManager* GameSceneManager(Game* game)
{
	return &game->managers.scene_manager;
}

void SceneManagerUpdate(SceneManager* scene_manager, Game* game, double dt)
{
	Scene* next = scene_manager->next;
	Scene* current = scene_manager->current;

	if (next)
	{
		if (current)
		{
			current->callbacks.exit(game, current->context);
			//current->callbacks.destroy(game, current->context);
		}

		scene_manager->current = next;
		scene_manager->next = NULL;

		if (scene_manager->current->is_ready == STI_FALSE)
		{
			scene_manager->current->callbacks.ready(game, scene_manager->current->context);
			scene_manager->current->is_ready = STI_TRUE;
		}

		scene_manager->current->callbacks.enter(game, scene_manager->current->context);
	}

	if (scene_manager->current)
	{
		scene_manager->current->callbacks.update(game, scene_manager->current->context, dt);
	}	
}