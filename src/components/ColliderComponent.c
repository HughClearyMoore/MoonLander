#include "ECS/component.h"

#include <assert.h>

#include "MLCore.h"

Collider ColliderCreate(ECS* ecs, dSpaceID space, Entity_t rigid_body_owner, dGeomID geom)
{
	Collider collider = { 0 };

	collider.internal.body = rigid_body_owner;
	collider.internal.geom = geom;
	collider.internal.space = space;

	RigidBody* rb = MLECSGetComponentRigidBody(ecs, rigid_body_owner);

	assert(rb);

	dGeomSetBody(geom, rb->internal.body);

	// register ourselves with the rigid body
	DynArrayPush(&rb->internal.attached_geoms, &geom);

	dSpaceAdd(space, geom);

	return collider;
}

static STI_BOOL geomFinder(const void* a, const void* b)
{
	return *(dGeomID*)a == *(dGeomID*)b;
}

COMPONENT_DESTROY(Collider)
{
	Collider* collider = (Collider*)component;

	// unregister myself

	RigidBody* rb = MLECSGetComponentRigidBody(collider->internal.ecs, collider->internal.body);

	if (rb)
	{		
		// unregister ourselves
		DynArray* attached = &rb->internal.attached_geoms;

		// the ol swap and pop
		STI_Finder finder = DynArrayFind(attached, &collider->internal.geom, &geomFinder);

		assert(finder.is_found);

		const size_t idx = finder.result.index;
		const size_t back_idx = DynArraySize(attached) - 1;
		
		DynArraySwap(attached, idx, back_idx, dGeomID);

		DynArrayPopBack(attached);
	}	

	dSpaceRemove(collider->internal.space, collider->internal.geom);
	dGeomSetBody(collider->internal.geom, NULL);
	dGeomDestroy(collider->internal.geom);
}