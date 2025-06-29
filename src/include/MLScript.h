#pragma once

#include "MLCore.h"
// what is a script?

typedef void(*OnUpdate_t)(Game* game_ctx, void* ctx, double dt);
typedef void(*OnReady_t)(Game* game_ctx, void* ctx);
typedef void(*OnCreate_t)(Game* game_ctx, void** ctx_ptr);
typedef void(*OnDestroy_t)(Game* game_ctx, void* ctx);

typedef struct Script
{
	void* ctx;
	OnUpdate_t update;
	OnReady_t ready;
	OnDestroy_t destroy;
	OnCreate_t create;
} Script;

#define SCRIPT_END

#define SCRIPT_START(name) \
	void ScriptCreate##name(Game* game_ctx, void** ctx_ptr); \
	void OnReady##name(Game* game_ctx, void* ctx); \
	void OnDestroy##name(Game* game_ctx, void* ctx);

#define SCRIPT(name) \
	SCRIPT_START(name)

#define SCRIPT_HAS_UPDATE(name) \
	void OnUpdate##name(Game* game_ctx, void* ctx, double dt);

	
#include "../defs/MLScripts.defs"

#undef SCRIPT
#undef SCRIPT_END
#undef SCRIPT_START

#undef SCRIPT_HAS_UPDATE

#define SCRIPT(name) \
	Script ScriptLink##name();

#define SCRIPT_END
#define SCRIPT_HAS_UPDATE(name)

#include "../defs/MLScripts.defs"

#undef SCRIPT
#undef SCRIPT_END

#undef SCRIPT_HAS_UPDATE


#define CREATE_FUNCTION(name) void ScriptCreate##name(Game* game_ctx, void** ctx_ptr)
#define READY_FUNCTION(name) void OnReady##name(Game* game_ctx, void* ctx)
#define DESTROY_FUNCTION(name) void OnDestroy##name(Game* game_ctx, void* ctx)
#define UPDATE_FUNCTION(name) void OnUpdate##name(Game* game_ctx, void* ctx, double dt)
