#pragma once

#include <stdlib.h>

#include "ECS/ECSConfig.h"

typedef struct Game Game;
typedef void* ScriptContext;

typedef void(*OnUpdate_t)(Game* game_ctx, Entity_t entity, ScriptContext ctx, double dt);
typedef void(*OnReady_t)(Game* game_ctx, Entity_t entity, ScriptContext ctx);
typedef void(*OnCreate_t)(Game* game_ctx, ScriptContext* ctx_ptr);
typedef void(*OnDestroy_t)(Game* game_ctx, Entity_t entity, ScriptContext ctx);

typedef void(*OnCollisionEnter_t)(Game* game_ctx, Entity_t, ScriptContext ctx, Entity_t other);
typedef void(*OnCollision_t)(Game* game_ctx, Entity_t, ScriptContext ctx, Entity_t other);
typedef void(*OnCollisionExit_t)(Game* game_ctx, Entity_t, ScriptContext ctx, Entity_t other);


typedef struct MLScript
{	
	OnUpdate_t update;
	OnReady_t ready;
	OnDestroy_t destroy;
	OnCreate_t create;

	OnCollisionEnter_t collision_enter;
	OnCollision_t collision;
	OnCollisionExit_t collision_exit;
} MLScript;


#define SCRIPT_END

#define SCRIPT_START(name) \
	void ScriptCreate##name(Game* game_ctx, ScriptContext* ctx_ptr); \
	void OnReady##name(Game* game_ctx, Entity_t entity, ScriptContext ctx); \
	void OnDestroy##name(Game* game_ctx, Entity_t entity, ScriptContext ctx);

#define SCRIPT(name) \
	SCRIPT_START(name)

#define SCRIPT_HAS_UPDATE(name) \
	void OnUpdate##name(Game* game_ctx, Entity_t entity, ScriptContext ctx, double dt);

#define SCRIPT_HAS_COLLISION_ENTER(name) \
	void OnCollisionEnter##name(Game* game_ctx, Entity_t entity, ScriptContext ctx, Entity_t other);

#define SCRIPT_HAS_COLLISION(name) \
	void OnCollision##name(Game* game_ctx, Entity_t entity, ScriptContext ctx, Entity_t other);

#define SCRIPT_HAS_COLLISION_EXIT(name) \
	void OnCollisionExit##name(Game* game_ctx, Entity_t entity, ScriptContext ctx, Entity_t other);

#include "../defs/MLScripts.defs"

#undef SCRIPT
#undef SCRIPT_START

#undef SCRIPT_HAS_UPDATE
#define SCRIPT_HAS_UPDATE(name)

#undef SCRIPT_HAS_COLLISION_ENTER
#define SCRIPT_HAS_COLLISION_ENTER(name)

#undef SCRIPT_HAS_COLLISION
#define SCRIPT_HAS_COLLISION(name)

#undef SCRIPT_HAS_COLLISION_EXIT
#define SCRIPT_HAS_COLLISION_EXIT(name)


#define SCRIPT(name) \
	MLScript ScriptLink##name();

#include "../defs/MLScripts.defs"

#undef SCRIPT

#define CREATE_FUNCTION(name) void ScriptCreate##name(Game* game_ctx, ScriptContext* ctx_ptr)
#define READY_FUNCTION(name) void OnReady##name(Game* game_ctx, Entity_t entity, ScriptContext ctx)
#define DESTROY_FUNCTION(name) void OnDestroy##name(Game* game_ctx, Entity_t entity, ScriptContext ctx)
#define UPDATE_FUNCTION(name) void OnUpdate##name(Game* game_ctx, Entity_t entity, ScriptContext ctx, double dt)
#define COLLISION_ENTER_FUNCTION(name) void OnCollisionEnter##name(Game* game_ctx, Entity_t entity, ScriptContext ctx, Entity_t other);
#define COLLISION_FUNCTION(name) void OnCollision##name(Game* game_ctx, Entity_t entity, ScriptContext ctx, Entity_t other);
#define COLLISION_EXIT_FUNCTION(name) void OnCollisionExit##name(Game* game_ctx, Entity_t entity, ScriptContext ctx, Entity_t other);

#define GET_INPUT &game_ctx->input;

#define DEFAULT_CREATE_FUNCTION(name, context_type) CREATE_FUNCTION(name) \
{ \
	context_type* ctx = calloc(1, sizeof(context_type));\
	*ctx_ptr = ctx; \
}