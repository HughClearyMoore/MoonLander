#pragma once

#include "ECS/System.h"

typedef struct Game Game;

typedef struct ScriptSystem
{
	MLSystem* system;
} ScriptSystem;

ScriptSystem ScriptSystemCreate(Game* game);
void ScriptSystemUpdate(Game* game, ScriptSystem* system, double dt);
void ScriptSystemDestroy(Game* game, ScriptSystem* system);