#include "systems/PhysicsSystem.h"

#include <assert.h>
#include <stdio.h>

#include <cglm/cglm.h>

#include "MLCore.h"
#include "ECS/Component.h"

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

		transform->previous.position.prev_x = transform->position.x;
		transform->previous.position.prev_y = transform->position.y;
		transform->previous.position.prev_z = transform->position.z;

		transform->previous.rotation.prev_x = transform->rotation.x;
		transform->previous.rotation.prev_y = transform->rotation.y;
		transform->previous.rotation.prev_z = transform->rotation.z;
		transform->previous.rotation.prev_w = transform->rotation.w;		
	}

	dSpaceCollide(physics_system->space, physics_system, &CollisionCallback);
	dWorldStep(physics_system->world, dt);

	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];

		Transform* transform = MLECSGetComponentTransform(ecs, e);
		RigidBody* rb = MLECSGetComponentRigidBody(ecs, e);

		const dReal* pos = dBodyGetPosition(rb->internal.body);
		const dReal* rot = dBodyGetQuaternion(rb->internal.body);
		
		vec3 world_pos = { (float)pos[0], (float)pos[1], (float)pos[2] };
		versor world_rot = { (float)rot[1], (float)rot[2], (float)rot[3], (float)rot[0] };

		Parent* parent = MLECSGetComponentParent(ecs, e);

		if (parent)
		{
			EntityStatus parent_status = MLEntityManagerGetStatus(ecs->managers.entity_manager, parent->parent);

			if (parent_status.alive && (parent_status.generation == parent->generation))
			{
				mat4 parent_world;
				TransformGetWorldTransform(ecs, parent->parent, parent_world);
				
				vec3 parent_world_pos = { parent_world[3][0], parent_world[3][1], parent_world[3][2]};

				glm_vec3_sub(world_pos, parent_world_pos, world_pos);

				versor parent_rot;
				glm_mat4_quat(parent_world, parent_rot);

				versor parent_rot_inv;
				glm_quat_conjugate(parent_rot, parent_rot_inv);


				versor local_rot;
				glm_quat_mul(parent_rot_inv, world_rot, local_rot);

				transform->position.x = world_pos[0];
				transform->position.y = world_pos[1];
				transform->position.z = world_pos[2];

				transform->rotation.x = local_rot[0];
				transform->rotation.y = local_rot[1];
				transform->rotation.z = local_rot[2];
				transform->rotation.w = local_rot[3];

			}
			else
			{
				transform->position.x = world_pos[0];
				transform->position.y = world_pos[1];
				transform->position.z = world_pos[2];

				transform->rotation.x = world_rot[0];
				transform->rotation.y = world_rot[1];
				transform->rotation.z = world_rot[2];
				transform->rotation.w = world_rot[3];
			}
		}
		else
		{

			transform->position.x = world_pos[0];
			transform->position.y = world_pos[1];
			transform->position.z = world_pos[2];

			transform->rotation.x = world_rot[0];
			transform->rotation.y = world_rot[1];
			transform->rotation.z = world_rot[2];
			transform->rotation.w = world_rot[3];
		}
	}

	dJointGroupEmpty(physics_system->joint_group);
}