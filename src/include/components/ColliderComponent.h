#pragma once

#include <ode/ode.h>

#include <Utility.h>

typedef struct Game Game;
typedef struct RigidBody RigidBody;

typedef struct ColliderData
{
	Entity_ID_t entity_id;
	STI_BOOL is_static;
} ColliderData;

typedef struct Collider
{
	struct
	{
		dGeomID geom;		
		dSpaceID space;
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

Collider ColliderDynamicCreate(Game* game, 
	Entity_t rigid_body_owner, 
	dGeomID geom,
	double restitution,
	double friction,
	double bounce_vel);

Collider ColliderStaticCreate(Game* game,
	Entity_t owner,
	dGeomID geom,
	double restitution,
	double friction,
	double bounce_vel);