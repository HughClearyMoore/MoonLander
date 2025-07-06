#pragma once

#include "ECS/System.h"

typedef struct Game Game;

typedef struct NameSystem
{
	MLSystem* system;
} NameSystem;

NameSystem NameSystemCreate(Game* game);

Entity_t NameSystemFind(Game* game, const char* name);