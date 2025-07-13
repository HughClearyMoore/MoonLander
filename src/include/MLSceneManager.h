#pragma once

#include <DynArray.h>
#include <HashMap.h>


#include "MLScene.h"

typedef Game Game;

typedef struct SceneManager
{
	HashMap scene_factory; //HashMap<string, Scene>
	DynArray live_scenes; //DynArray<Scene*>

	Scene* current;
	Scene* next;
} SceneManager;

SceneManager SceneManagerCreate();
void SceneManagerDestroy(SceneManager* scene_manager);
Scene* SceneManagerCreateScene(SceneManager* scene_manager, Game* game, const char* name);
void SceneManagerDestroyScene(SceneManager* scene_manager, Game* game, Scene* scene);
void SceneManagerUpdate(SceneManager* scene_manager, Game* game, double dt);
void SceneManagerSetScene(SceneManager* scene_manager, Game* game, const char* name);

void SceneManagerSwitch(SceneManager* scene_manager, Game* game, const char* name);
SceneManager* GameSceneManager(Game* game);