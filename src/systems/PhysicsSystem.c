#include "systems/PhysicsSystem.h"

#include <assert.h>
#include <stdio.h>

#include "MLCore.h"
#include "ECS/Component.h"


/*
	dInitODE2(0);

	dWorldID world = dWorldCreate();

	dBodyID body = dBodyCreate(world);
	dBodySetPosition(body, 0.0, 1.0, 0.0);

	const dReal* pos = dBodyGetPosition(body);
	printf("Body position: x=%.2f, y=%.2f, z=%.2f\n", pos[0], pos[1], pos[2]);

	dBodyDestroy(body);
	dWorldDestroy(world);
	dCloseODE();

*/

PhysicsSystem PhysicsSystemCreate(Game* game)
{
	PhysicsSystem system = { 0 };
	Signature_t signature = 0;
	signature |= 1 << ENUM_COMPONENT_Transform;
	signature |= 1 << ENUM_COMPONENT_RigidBody;

	system.system = MLECSNewSystem(&game->ecs, signature);

	dInitODE2(0);

	system.joint_group = dJointGroupCreate(0);
	system.world = dWorldCreate();
	system.space = dSimpleSpaceCreate(0);

	dWorldSetGravity(system.world, 0.0, -0.1, 0.0);

	return system;
}

void PhysicsSystemDestroy(Game* game)
{
	PhysicsSystem* physics_system = &game->ecs.systems.physics;

	Entity_t* entities = physics_system->system->entities.data;
	const size_t sz = DynArraySize(&physics_system->system->entities);

	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];
		MLECSRemoveComponentRigidBody(&game->ecs, e);
	}

	dJointGroupDestroy(physics_system->joint_group);
	dSpaceDestroy(physics_system->space);
	dWorldDestroy(physics_system->world);
	dCloseODE();
}

static void CollisionCallback(void* data, dGeomID o1, dGeomID o2)
{
	printf("COLLISION!\n");
}

void PhysicsSystemUpdate(Game* game, PhysicsSystem* physics_system, double dt)
{
	MLSystem* system = physics_system->system;
	ECS* ecs = &game->ecs;

	const size_t sz = DynArraySize(&system->entities);
	Entity_t* entities = system->entities.data;

	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];

		Transform* transform = MLECSGetComponentTransform(ecs, e);
		RigidBody* rb = MLECSGetComponentRigidBody(ecs, e);

		transform->prev_x = transform->x;
		transform->prev_y = transform->y;
		transform->prev_z = transform->z;
	}

	dSpaceCollide(physics_system->space, physics_system, &CollisionCallback);
	dWorldStep(physics_system->world, dt);

	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];

		Transform* transform = MLECSGetComponentTransform(ecs, e);
		RigidBody* rb = MLECSGetComponentRigidBody(ecs, e);

		const dReal* pos = dBodyGetPosition(rb->internal.body);
		
		transform->x = pos[0];
		transform->y = pos[1];
		transform->z = pos[2];
	}

	dJointGroupEmpty(physics_system->joint_group);
}