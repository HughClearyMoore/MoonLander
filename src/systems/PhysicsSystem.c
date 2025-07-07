#include "systems/PhysicsSystem.h"

#include <assert.h>
#include <stdio.h>

#include <cglm/cglm.h>

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

	//Entity_t* entities = physics_system->system->entities.data;
	//const size_t sz = DynArraySize(&physics_system->system->entities);
	
	Entity_t* entity = NULL;
	while (DynArraySize(&physics_system->system->entities) > 0)
	{
		Entity_t* entity = DynArrayBack(&physics_system->system->entities);

		MLECSRemoveComponentRigidBody(&game->ecs, *entity);	
	}

	/*
	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[0];
		MLECSRemoveComponentRigidBody(&game->ecs, e);
	}
	*/
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
		
		vec3 world_pos = { (float)pos[0], (float)pos[1], (float)pos[2] };
		Parent* parent = MLECSGetComponentParent(ecs, e);

		if (parent)
		{
			EntityStatus parent_status = MLEntityManagerGetStatus(ecs->managers.entity_manager, parent->parent);

			if (parent_status.alive && (parent_status.generation == parent->generation))
			{
				Transform parent_world = TransformGetWorldTransform(ecs, parent->parent);
				vec3 parent_world_pos = { parent_world.x, parent_world.y, parent_world.z };

				glm_vec3_sub(world_pos, parent_world_pos, world_pos);
			}
		}

		transform->x = world_pos[0];
		transform->y = world_pos[1];
		transform->z = world_pos[2];
	}

	dJointGroupEmpty(physics_system->joint_group);
}