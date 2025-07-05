#pragma once

#include "ECS/System.h"

typedef struct Game Game;

typedef struct RenderingSystem
{
	MLSystem* system;
	Game* game;
} RenderingSystem;

RenderingSystem RenderingSystemCreate(Game* game);

void RenderingSystemUpdate(Game* game, RenderingSystem* rendering_system, double dt);