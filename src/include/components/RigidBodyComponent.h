#pragma once

#include <ode/ode.h>

#include <DynArray.h>

#include "ECS/ECSConfig.h"

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

RigidBody RigidBodyCreate(ECS* ecs, Transform* transform, dWorldID world, dMass* mass);
void RigidBodyAddTorque(ECS* ecs, Entity_t entity, vec3 axis, float torque_nm);