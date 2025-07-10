#pragma once

#include <ode/ode.h>

#include "../ECS/System.h"

typedef struct Game Game;

typedef struct PhysicsSystem
{
	MLSystem* system;
	MLSystem* collision_system;
	dWorldID world;
	dSpaceID space;
	dJointGroupID joint_group;

} PhysicsSystem;

PhysicsSystem PhysicsSystemCreate(Game* game);
void PhysicsSystemDestroy(Game* game);

void PhysicsSystemUpdate(Game* game, PhysicsSystem* physics_system, double dt);
dSpaceID PhysicsSystemCurrentSpace(Game* game);
dWorldID PhysicsSystemCurrentWorld(Game* game);