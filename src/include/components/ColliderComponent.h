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
	struct
	{
		double restitution; // bounciness [0,1], 0 = not bouncy, 1 = absolute boing
		double friction; // [0, inf], 0 = no friction, inf = no sliding. For inf pass dInfinity
		double bounce_vel; // [0, inf], minimum required velocity in m/s to bounce.
		double soft_erp; // [0,1], how hard it pushes back
		double soft_cfm; // [0,1], compliance, imagine I whacked a pillow in between the object. set to typically small numbers like 0.01
		// for soft_erp and soft_cfm to do anything you need to set the dContactSoftCFM and dContactSoftERP bitmasks
	} properties;
} Collider;

Collider ColliderCreate(ECS* ecs, dSpaceID space, Entity_t rigid_body_owner, dGeomID geom);