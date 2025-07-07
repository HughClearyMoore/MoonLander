#include "ECS/component.h"

#include "MLCore.h"

RigidBody RigidBodyCreate(ECS* ecs, dWorldID world, dMass* mass)
{
	RigidBody body = { 0 };

	body.internal.world = world;

	body.internal.body = dBodyCreate(world);
	dBodySetMass(body.internal.body, mass);

	body.mass = mass->mass;

	body.internal.attached_geoms = DynArrayCreate(sizeof(dGeomID), 0, NULL);	
	body.internal.ecs = ecs;

	return body;
}


COMPONENT_DESTROY(RigidBody)
{
	RigidBody* rb = (RigidBody*)component;	

	dGeomID* attached = rb->internal.attached_geoms.data;
	const size_t sz = DynArraySize(&rb->internal.attached_geoms);

	
	dBodyDestroy(rb->internal.body); // no body told me but this removes attached geoms for you // I'll probably untrack them at some point

	DynArrayDestroy(&rb->internal.attached_geoms);
}