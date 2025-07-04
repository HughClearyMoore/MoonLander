#pragma once

#include "MLCore.h"

typedef struct RenderingSystem
{
	MLSystem* system;
	Game* game;
} RenderingSystem;

RenderingSystem* RenderingSystemCreate(Game* game);

void RenderingSystemUpdate(RenderingSystem* rendering_system, double dt);
void RenderingSystemDestroy(RenderingSystem* rendering_system);