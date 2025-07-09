#include "ECS/component.h"

#include <cglm/cglm.h>

#include "MLCore.h"

RigidBody RigidBodyCreate(ECS* ecs, Transform* transform, dWorldID world, dMass* mass)
{
	RigidBody body = { 0 };

	body.internal.world = world;

	body.internal.body = dBodyCreate(world);	

	dBodySetMass(body.internal.body, mass);
	dBodySetPosition(body.internal.body,
		transform->position.x,
		transform->position.y,
		transform->position.z);

	dQuaternion rot = {
		transform->rotation.w,
		transform->rotation.x,
		transform->rotation.y,
		transform->rotation.z
	};

	dBodySetQuaternion(body.internal.body,
		rot);

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