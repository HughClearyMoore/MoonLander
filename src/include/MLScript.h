#pragma once

typedef struct Game Game;
typedef void* ScriptContext;

typedef void(*OnUpdate_t)(Game* game_ctx, ScriptContext ctx, double dt);
typedef void(*OnReady_t)(Game* game_ctx, ScriptContext ctx);
typedef void(*OnCreate_t)(Game* game_ctx, ScriptContext* ctx_ptr);
typedef void(*OnDestroy_t)(Game* game_ctx, ScriptContext ctx);

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
	void ScriptCreate##name(Game* game_ctx, ScriptContext* ctx_ptr); \
	void OnReady##name(Game* game_ctx, ScriptContext ctx); \
	void OnDestroy##name(Game* game_ctx, ScriptContext ctx);

#define SCRIPT(name) \
	SCRIPT_START(name)

#define SCRIPT_HAS_UPDATE(name) \
	void OnUpdate##name(Game* game_ctx, ScriptContext ctx, double dt);

	
#include "../defs/MLScripts.defs"

#undef SCRIPT
#undef SCRIPT_START

#undef SCRIPT_HAS_UPDATE
#define SCRIPT_HAS_UPDATE(name)

#define SCRIPT(name) \
	Script ScriptLink##name();

#include "../defs/MLScripts.defs"

#undef SCRIPT

#define CREATE_FUNCTION(name) void ScriptCreate##name(Game* game_ctx, ScriptContext* ctx_ptr)
#define READY_FUNCTION(name) void OnReady##name(Game* game_ctx, ScriptContext ctx)
#define DESTROY_FUNCTION(name) void OnDestroy##name(Game* game_ctx, ScriptContext ctx)
#define UPDATE_FUNCTION(name) void OnUpdate##name(Game* game_ctx, ScriptContext ctx, double dt)
#define GET_INPUT &game_ctx->input;