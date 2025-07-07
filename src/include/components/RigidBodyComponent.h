#pragma once

#include <ode/ode.h>

#include <DynArray.h>

typedef struct ECS ECS;

typedef struct RigidBody
{	
	struct
	{
		dBodyID body;
		dMass mass;
		dWorldID world;
		DynArray attached_geoms; // DynArray<dGeomID>
		ECS* ecs;
	} internal;

	double mass;
} RigidBody;

RigidBody RigidBodyCreate(ECS* ecs, dWorldID world, dMass* mass);