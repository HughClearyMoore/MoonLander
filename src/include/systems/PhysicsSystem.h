#pragma once

#include <ode/ode.h>

#include <DynArray.h>
#include <HashMap.h>

#include "../ECS/System.h"



#define COLLISION_MAX_CONTACTS 8

typedef struct Game Game;
typedef struct ECS ECS;

typedef struct CollisionTracking
{
	HashMap collisions; // HashMap<size_t, STI_BOOL>

	DynArray marked_collisions; // DynArray<CollidingPair>

} CollisionTracking;

typedef struct PhysicsSystem
{
	MLSystem* system;
	MLSystem* collision_system;
	dWorldID world;
	dSpaceID space;
	dJointGroupID joint_group;

	CollisionTracking collision_tracking;

} PhysicsSystem;

PhysicsSystem PhysicsSystemCreate(Game* game, ECS* ecs);
void PhysicsSystemDestroy(Game* game, ECS* ecs);

void PhysicsSystemUpdate(Game* game, PhysicsSystem* physics_system, double dt);
dSpaceID PhysicsSystemCurrentSpace(Game* game);
dWorldID PhysicsSystemCurrentWorld(Game* game);