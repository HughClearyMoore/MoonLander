#pragma once

#include <DynArray.h>

#include "MLScript.h"

typedef struct ScriptManager
{
	// DynArray<Script*>
	DynArray scripts;
} ScriptManager;

#define SCRIPT(name) SCRIPT_ENUM_##name,

typedef enum Script_t
{
#include "../defs/MLScripts.defs"
	SCRIPT_ENUM_COUNT
} Script_t;

#undef SCRIPT

const char* MLGetScriptName(Script_t script_type);

ScriptManager MLScriptManagerCreate();
void MLScriptManagerDestroy(ScriptManager* manager);

MLScript* MLScriptManagetGet(ScriptManager* manager, Script_t script_type);