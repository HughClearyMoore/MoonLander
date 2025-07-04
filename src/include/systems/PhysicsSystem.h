#pragma once

#include "ECS/ecs.h"

typedef struct PhysicsSystem
{
	MLSystem* system;
	ECS* ecs;
} PhysicsSystem;

PhysicsSystem PhysicsSystemCreate(ECS* ecs);

void PhysicsSystemUpdate(PhysicsSystem* physics_system, double dt);