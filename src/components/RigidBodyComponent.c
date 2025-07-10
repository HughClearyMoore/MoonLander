#include "ECS/component.h"

#include <cglm/cglm.h>

#include "MLCore.h"

RigidBody RigidBodyCreate(ECS* ecs, Entity_t entity, dWorldID world, dMass* mass)
{
	RigidBody body = { 0 };

	Transform* transform = MLECSGetComponentTransform(ecs, entity);

	assert(transform && "rigid body requires a transform");

	body.internal.world = world;

	body.internal.body = dBodyCreate(world);

	dBodySetMass(body.internal.body, mass);
	dBodySetPosition(body.internal.body,
		transform->position.x,
		transform->position.y,
		transform->position.z);

	transform->previous.position.prev_x = transform->position.x;
	transform->previous.position.prev_y = transform->position.y;
	transform->previous.position.prev_z = transform->position.z;

	transform->previous.rotation.prev_x = transform->rotation.x;
	transform->previous.rotation.prev_y = transform->rotation.y;
	transform->previous.rotation.prev_z = transform->rotation.z;
	transform->previous.rotation.prev_w = transform->rotation.w;

	dQuaternion rot = {
		transform->rotation.w,
		transform->rotation.x,
		transform->rotation.y,
		transform->rotation.z
	};

	dBodySetQuaternion(body.internal.body,
		rot);

	body.mass = mass->mass;

	return body;
}


COMPONENT_DESTROY(RigidBody)
{
	RigidBody* rb = (RigidBody*)component;	
	
	dBodyDestroy(rb->internal.body); // no body told me but this removes attached geoms for you // I'll probably untrack them at some point
}

void RigidBodyAddTorque(ECS* ecs, Entity_t entity, vec3 axis, float torque_nm)
{
	RigidBody* rb = MLECSGetComponentRigidBody(ecs, entity);

	if (rb == NULL) return;

	versor world_rot;
	TransformGetWorldVersor(ecs, entity, world_rot);
	
	vec3 torque_direction;
	glm_quat_rotatev(world_rot, axis, torque_direction);

	glm_vec3_normalize(torque_direction);
	glm_vec3_scale(torque_direction, torque_nm, torque_direction);

	dBodyAddTorque(rb->internal.body,
		torque_direction[0],
		torque_direction[1],
		torque_direction[2]);
}