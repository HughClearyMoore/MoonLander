#pragma once

#include "../MLScriptManager.h"

typedef struct Game Game;

typedef struct Script
{
	MLScript* script;
	ScriptContext context;
} Script;

Script ScriptComponentCreate(Game* game, Script_t type);