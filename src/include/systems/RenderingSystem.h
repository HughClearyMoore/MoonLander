#pragma once

#include "ECS/System.h"

typedef struct ECS ECS;
typedef struct Game Game;

typedef struct RenderingSystem
{
	MLSystem* system;
	Entity_t camera;
} RenderingSystem;

RenderingSystem RenderingSystemCreate(Game* game, ECS* ecs);

void RenderingSystemUpdate(Game* game, RenderingSystem* rendering_system, double dt, double alpha);

void RenderingSystemSetCamera(RenderingSystem* rendering_system, Entity_t camera_entity);