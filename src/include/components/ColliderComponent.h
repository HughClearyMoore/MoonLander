#pragma once

#include <ode/ode.h>

typedef struct ECS ECS;
typedef struct RigidBody RigidBody;

typedef struct Collider
{
	struct
	{
		dGeomID geom;
		Entity_t body;
		dSpaceID space;
		ECS* ecs;
	} internal;
} Collider;

Collider ColliderCreate(ECS* ecs, dSpaceID space, Entity_t rigid_body_owner, dGeomID geom);