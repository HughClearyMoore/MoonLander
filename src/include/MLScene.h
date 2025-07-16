#pragma once

#include <STI_String.h>
#include <Utility.h>

typedef struct ECS ECS;
typedef struct Game Game;
typedef void* SceneContext;

typedef void(*SceneCreate)(Game* game, SceneContext* context);
typedef void(*SceneReady)(Game* game, SceneContext context);
typedef void(*SceneUpdate)(Game* game, SceneContext context, double dt);
typedef void(*SceneDestroy)(Game* game, SceneContext context);
typedef void(*SceneEnter)(Game* game, SceneContext context);
typedef void(*SceneExit)(Game* game, SceneContext context);

typedef struct Scene
{
	STI_String name;

	ECS* ecs;
	SceneContext context;
	STI_BOOL is_ready;

	struct
	{
		SceneCreate create;
		SceneReady ready;
		SceneUpdate update;
		SceneDestroy destroy;
		SceneEnter enter;
		SceneExit exit;
	} callbacks;

} Scene;

#define SCENE_CREATE_FUNCTION(name) void SceneCreate##name(Game* game, SceneContext* context)
#define SCENE_READY_FUNCTION(name) void SceneReady##name(Game* game, SceneContext* context)
#define SCENE_UPDATE_FUNCTION(name) void SceneUpdate##name(Game* game, SceneContext* context, double dt)
#define SCENE_DESTROY_FUNCTION(name) void SceneDestroy##name(Game* game, SceneContext* context)
#define SCENE_ENTER_FUNCTION(name) void SceneEnter##name(Game* game, SceneContext* context)
#define SCENE_EXIT_FUNCTION(name) void SceneExit##name(Game* game, SceneContext* context)

#define SCENE(name) \
SCENE_CREATE_FUNCTION(name); \
SCENE_READY_FUNCTION(name); \
SCENE_UPDATE_FUNCTION(name); \
SCENE_DESTROY_FUNCTION(name); \
SCENE_ENTER_FUNCTION(name); \
SCENE_EXIT_FUNCTION(name);

#include "../defs/MLScenes.defs"

#undef SCENE


#define SCENE_DEFAULT_CREATE_FUNCTION(name, context_type) SCENE_CREATE_FUNCTION(name) \
{ \
	context_type* ctx = calloc(1, sizeof(context_type));\
	*context = ctx; \
}