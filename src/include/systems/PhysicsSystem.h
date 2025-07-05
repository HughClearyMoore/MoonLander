#pragma once

#include "../ECS/System.h"

typedef struct Game Game;

typedef struct PhysicsSystem
{
	MLSystem* system;
	Game* game;
} PhysicsSystem;

PhysicsSystem PhysicsSystemCreate(Game* game);

void PhysicsSystemUpdate(PhysicsSystem* physics_system, double dt);