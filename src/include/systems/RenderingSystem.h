#pragma once

#include "ECS/System.h"

typedef struct Game Game;

typedef struct RenderingSystem
{
	MLSystem* system;
	Entity_t camera;
} RenderingSystem;

RenderingSystem RenderingSystemCreate(Game* game);
void RenderingSystemInitialise(RenderingSystem* rendering_system, Game* game);

void RenderingSystemUpdate(Game* game, RenderingSystem* rendering_system, double dt, double alpha);