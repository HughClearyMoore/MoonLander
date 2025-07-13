#pragma once

#include "ECS/System.h"

typedef struct ECS ECS;
typedef struct Game Game;

typedef struct NameSystem
{
	MLSystem* system;
} NameSystem;

NameSystem NameSystemCreate(Game* game, ECS* ecs);

Entity_t NameSystemFind(Game* game, const char* name);