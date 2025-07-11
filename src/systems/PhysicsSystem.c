#include "systems/PhysicsSystem.h"

#include <assert.h>
#include <stdio.h>

#include <cglm/cglm.h>

#include "MLCore.h"
#include "ECS/Component.h"

static inline size_t CreatePairIndex(Entity_t e1, Entity_t e2)
{
	if (e1 > e2)
	{
		Entity_t tmp = e1;
		e1 = e2;
		e2 = tmp;
	}

	return ((size_t)e1 << (8 * sizeof(Entity_t))) | (size_t)e2;
}

PhysicsSystem PhysicsSystemCreate(Game* game)
{
	PhysicsSystem system = { 0 };
	Signature_t signature = 0;
	signature |= 1 << ENUM_COMPONENT_Transform;
	signature |= 1 << ENUM_COMPONENT_RigidBody;

	system.system = MLECSNewSystem(&game->ecs, signature);

	signature = 0;
	signature |= 1 << ENUM_COMPONENT_Collider;
	system.collision_system = MLECSNewSystem(&game->ecs, signature);

	dInitODE2(0);

	system.joint_group = dJointGroupCreate(0);
	system.world = dWorldCreate();
	system.space = dSimpleSpaceCreate(0);

	dWorldSetGravity(system.world, 0.0, -1.625, 0.0);
	
	system.collision_tracking.collisions = HashMapCreate(
		HASHMAP_KEY_INT,
		sizeof(STI_BOOL),
		1024,
		&HashMapIntHash,
		&HashMapIntCmp,
		NULL);
	

	system.collision_tracking.marked_collisions = DynArrayCreate(sizeof(CollidingPair), MAX_ENTITIES, NULL);

	return system;
}

void PhysicsSystemDestroy(Game* game)
{
	PhysicsSystem* physics_system = &game->ecs.systems.physics;

	//Entity_t* entities = physics_system->system->entities.data;
	//const size_t sz = DynArraySize(&physics_system->system->entities);

	HashMapDestroy(&physics_system->collision_tracking.collisions);

	DynArrayDestroy(&physics_system->collision_tracking.marked_collisions);
	
	while (DynArraySize(&physics_system->collision_system->entities) > 0)
	{
		Entity_t* entity = DynArrayBack(&physics_system->collision_system->entities);

		MLECSRemoveComponentCollider(&game->ecs, *entity);
	}

	while (DynArraySize(&physics_system->system->entities) > 0)
	{
		Entity_t* entity = DynArrayBack(&physics_system->system->entities);
		
		MLECSRemoveComponentRigidBody(&game->ecs, *entity);	
	}

	dJointGroupDestroy(physics_system->joint_group);
	dSpaceDestroy(physics_system->space);
	dWorldDestroy(physics_system->world);
	dCloseODE();
}

static void ClearCollisionPairs(PhysicsSystem* physics_system)
{
	CollisionTracking* tracking = &physics_system->collision_tracking;

	DynArray* marked_collisions = &tracking->marked_collisions;
	HashMap* collisions = &tracking->collisions;

	const size_t pair_count = DynArraySize(marked_collisions);
	CollidingPair* pairs = (CollidingPair*)marked_collisions->data;

	for (size_t i = 0; i < pair_count; ++i)
	{
		CollidingPair pair = pairs[i];

		STI_BOOL* is_colliding = HashMapGet(collisions, &pair.pair_index);

		assert(is_colliding);

		*is_colliding = STI_FALSE;
	}

	// DynArray doesn't have clear but here's the next best thing
	marked_collisions->size = 0;
}

static void FindCollisionExits(PhysicsSystem* physics_system)
{
	CollisionTracking* tracking = &physics_system->collision_tracking;

	DynArray* marked_collisions = &tracking->marked_collisions;
	HashMap* collisions = &tracking->collisions;

	const size_t pair_count = DynArraySize(marked_collisions);
	CollidingPair* pairs = (CollidingPair*)marked_collisions->data;

	for (size_t i = 0; i < pair_count; ++i)
	{
		CollidingPair pair = pairs[i];		

		STI_BOOL* is_colliding = (STI_BOOL*)HashMapGet(collisions, &pair.pair_index);

		assert(is_colliding);

		if (*is_colliding == STI_FALSE)
		{
			// collision exit
		}
	}
}

static void OnEntityCollision(PhysicsSystem* physics_system, Entity_t entity_1, Entity_t entity_2)
{
	CollisionTracking* tracking = &physics_system->collision_tracking;

	const size_t pair_index = CreatePairIndex(entity_1, entity_2);
	HashMap* collisions = &tracking->collisions;

	STI_BOOL* is_colliding = HashMapGet(collisions, &pair_index);

	if (is_colliding)
	{
		// collision stay
	}
	else
	{
		// collision enter
	}

	STI_BOOL true_val = STI_TRUE;
	HashMapInsert(collisions, &pair_index, &true_val);

	CollidingPair pair = { .pair_index = pair_index };
	DynArrayPush(&tracking->marked_collisions, &pair);
}

static void CollisionCallback(void* data, dGeomID o1, dGeomID o2)
{	
	dContact contacts[COLLISION_MAX_CONTACTS];

	Game* game = (Game*)data;

	ColliderData* o1_data = dGeomGetData(o1);
	ColliderData* o2_data = dGeomGetData(o2);

	Collider* o1_collider = MLECSGetComponentCollider(GameECS(game), o1_data->entity_id.entity);
	Collider* o2_collider = MLECSGetComponentCollider(GameECS(game), o2_data->entity_id.entity);

	if (!(o1_collider && o2_collider))
	{
		assert(0 && "Collider missing");
	}

	size_t num_contacts = dCollide(o1, o2, COLLISION_MAX_CONTACTS, &contacts[0].geom, sizeof(dContact));

	if (num_contacts) // if num_contact > 0 then a collision has occurred
	{		
		Entity_t entity_1 = o1_data->entity_id.entity;
		Entity_t entity_2 = o2_data->entity_id.entity;

		if (entity_1 != entity_2)
		{
			OnEntityCollision(MLECSPhysicsSystem(game), entity_1, entity_2);
		}		
	}

	PhysicsSystem* physics = MLECSPhysicsSystem(game);

	for (int i = 0; i < num_contacts; ++i)
	{
		contacts[i].surface.mode = dContactBounce | dContactSoftERP | dContactSoftCFM;
		contacts[i].surface.mu = (o1_collider->properties.friction + o2_collider->properties.friction) / 2;
		contacts[i].surface.bounce = (o1_collider->properties.restitution + o2_collider->properties.restitution) / 2;
		contacts[i].surface.bounce_vel = (o1_collider->properties.bounce_vel + o2_collider->properties.bounce_vel) / 2;
		contacts[i].surface.soft_erp = 0.2;
		contacts[i].surface.soft_cfm = 10e-5;

		dJointID c = dJointCreateContact(physics->world, physics->joint_group, &contacts[i]);
		dBodyID b1 = dGeomGetBody(o1);
		dBodyID b2 = dGeomGetBody(o2);

		if (b1 || b2)
		{
			dJointAttach(c, b1, b2);
		}		
	}
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

	ClearCollisionPairs(physics_system);

	dSpaceCollide(physics_system->space, game, &CollisionCallback);

	FindCollisionExits(physics_system);

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

dSpaceID PhysicsSystemCurrentSpace(Game* game)
{
	return MLECSPhysicsSystem(game)->space;
}

dWorldID PhysicsSystemCurrentWorld(Game* game)
{
	return MLECSPhysicsSystem(game)->world;
}
