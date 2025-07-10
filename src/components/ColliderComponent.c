#include "ECS/component.h"

#include <assert.h>

#include "MLCore.h"

Collider ColliderDynamicCreate(Game* game,
	Entity_t rigid_body_owner,
	dGeomID geom,
	double restitution,
	double friction,
	double bounce_vel)
{
	Collider collider = { 0 };

	EntityStatus owner_status = MLEntityManagerGetStatus(MLECSEntityManager(game), rigid_body_owner);

	if (!owner_status.alive)
	{
		assert("Can't add a collider to a dead entity");
	}

	collider.internal.geom = geom;
	collider.internal.space = PhysicsSystemCurrentSpace(game);

	collider.properties.bounce_vel = bounce_vel;
	collider.properties.friction = friction;
	collider.properties.restitution = restitution;

	RigidBody* rb = MLECSGetComponentRigidBody(GameECS(game), rigid_body_owner);

	assert(rb);

	dGeomSetBody(geom, rb->internal.body);	


	dSpaceAdd(collider.internal.space, geom);


	ColliderData* data = calloc(1, sizeof(ColliderData));
	assert(data);
	
	data->entity_id.entity = rigid_body_owner;
	data->entity_id.generation = owner_status.generation;

	data->is_static = STI_FALSE;

	dGeomSetData(geom, data);

	return collider;
}

Collider ColliderStaticCreate(Game* game,
	Entity_t owner,
	dGeomID geom,
	double restitution,
	double friction,
	double bounce_vel)
{
	Collider collider = { 0 };
	
	collider.internal.geom = geom;

	

	collider.internal.space = PhysicsSystemCurrentSpace(game);

	collider.properties.bounce_vel = bounce_vel;
	collider.properties.friction = friction;
	collider.properties.restitution = restitution;

	dSpaceAdd(collider.internal.space, geom);

	ColliderData* data = calloc(1, sizeof(ColliderData));
	assert(data);

	EntityStatus owner_status = MLEntityManagerGetStatus(MLECSEntityManager(game), owner);

	if (!owner_status.alive)
	{
		assert("Can't add collider to dead entity");
	}

	data->entity_id.entity = owner;
	data->entity_id.generation = owner_status.generation;
	
	data->is_static = STI_TRUE;

	dGeomSetData(geom, data);

	return collider;
}

static STI_BOOL geomFinder(const void* a, const void* b)
{
	return *(dGeomID*)a == *(dGeomID*)b;
}

COMPONENT_DESTROY(Collider)
{
	Collider* collider = (Collider*)component;	

	ColliderData* collider_data = (ColliderData*)dGeomGetData(collider->internal.geom);

	if (collider_data->is_static)
	{
		dSpaceRemove(collider->internal.space, collider->internal.geom);
		dGeomDestroy(collider->internal.geom);
	}
	else
	{		
		dGeomSetBody(collider->internal.geom, 0);
		dSpaceRemove(collider->internal.space, collider->internal.geom);
		dGeomDestroy(collider->internal.geom);

	}

	free(collider_data);
}