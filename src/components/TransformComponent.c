#include "ECS/Component.h"

#include <stdio.h>

#include "MLCore.h"

COMPONENT_DESTROY(Transform)
{
	Transform* transform = (Transform*)component;

	printf("transform destroyed: %f, %f, %f\n", transform->position.x, transform->position.y, transform->position.z);

	*transform = (Transform){ 0 };
}

void TransformGetWorldTransform(ECS* ecs, Entity_t entity, mat4 out)
{
	glm_mat4_identity(out);

	Transform* t = MLECSGetComponentTransform(ecs, entity);

	if (t == NULL) return;

	vec3 pos = {
		(float)t->position.x,
		(float)t->position.y,
		(float)t->position.z
	};

	Transform result = *t;


	mat4 local;
	glm_mat4_identity(local);

	glm_translate(local, pos);

	mat4 rot;
	glm_quat_mat4((versor) {
		(float)t->rotation.x,
			(float)t->rotation.y,
			(float)t->rotation.z,
			(float)t->rotation.w
	}, rot);

	glm_mat4_mul(local, rot, local);

	glm_scale(local, (vec3) {
		(float)t->scale.scale_x,
			(float)t->scale.scale_y,
			(float)t->scale.scale_z
	});

	Parent* p = MLECSGetComponentParent(ecs, entity);

	if (p)
	{
		EntityStatus status = MLEntityManagerGetStatus(ecs->managers.entity_manager, p->parent);

		if (status.alive == STI_TRUE && status.generation == p->generation)
		{
			mat4 parent_world;			
			TransformGetWorldTransform(ecs, p->parent, parent_world);
			mat4 result;
			glm_mat4_mul(parent_world, local, result);
			glm_mat4_copy(result, out);

			return;
		}
	}

	glm_mat4_copy(local, out);
}

void TransformGetInterpolatedWorldTransform(ECS* ecs, Entity_t entity, double alpha, mat4 out)
{
	glm_mat4_identity(out);

	Transform* t = MLECSGetComponentTransform(ecs, entity);

	if (t == NULL) return;

	vec3 interp_pos = {
		(float)t->position.x,
		(float)t->position.y,
		(float)t->position.z
	};

	versor interp_rot = {
		(float)t->rotation.x,
		(float)t->rotation.y,
		(float)t->rotation.z,
		(float)t->rotation.w
	};

	Transform result = *t;

	if (MLECSGetComponentRigidBody(ecs, entity))
	{
		interp_pos[0] = (float)(t->previous.position.prev_x + (t->position.x - t->previous.position.prev_x) * alpha);
		interp_pos[1] = (float)(t->previous.position.prev_y + (t->position.y - t->previous.position.prev_y) * alpha);
		interp_pos[2] = (float)(t->previous.position.prev_z + (t->position.z - t->previous.position.prev_z) * alpha);

		interp_rot[0] = (float)(t->previous.rotation.prev_x + (t->rotation.x - t->previous.rotation.prev_x) * alpha);
		interp_rot[1] = (float)(t->previous.rotation.prev_y + (t->rotation.y - t->previous.rotation.prev_y) * alpha);
		interp_rot[2] = (float)(t->previous.rotation.prev_z + (t->rotation.z - t->previous.rotation.prev_z) * alpha);
		interp_rot[3] = (float)(t->previous.rotation.prev_w + (t->rotation.w - t->previous.rotation.prev_w) * alpha);
	}	

	glm_quat_normalize(interp_rot);


	mat4 local;
	glm_mat4_identity(local);

	glm_translate(local, interp_pos);

	mat4 rot;
	glm_quat_mat4(interp_rot, rot);

	glm_mat4_mul(local, rot, local);

	glm_scale(local, (vec3) {
			(float)t->scale.scale_x,
			(float)t->scale.scale_y,
			(float)t->scale.scale_z
	});

	Parent* p = MLECSGetComponentParent(ecs, entity);

	if (p)
	{
		EntityStatus status = MLEntityManagerGetStatus(ecs->managers.entity_manager, p->parent);

		if (status.alive == STI_TRUE && status.generation == p->generation)
		{
			mat4 parent_world;
			TransformGetInterpolatedWorldTransform(ecs, p->parent, alpha, parent_world);
			mat4 result;
			glm_mat4_mul(parent_world, local, result);
			glm_mat4_copy(result, out);

			return;
		}
	}

	glm_mat4_copy(local, out);
}

Transform TransformIdentity()
{
	Transform transform = {
		.position = {0.0, 0.0, 0.0},
		.previous = {0.0, 0.0, 0.0},
		.rotation = {0.0, 0.0, 0.0, 1.0},
		.previous = {0.0, 0.0, 0.0},
		.scale = {1.0, 1.0, 1.0}
	};

	
	return transform;
}

void TransformToMatrix(const Transform* transform, mat4 out)
{
	glm_mat4_identity(out);

	vec3 pos = {
		transform->position.x,
		transform->position.y,
		transform->position.z
	};

	glm_translate(out, pos);

	mat4 rot;
	glm_quat_mat4(
		(versor) {
			transform->rotation.x,
			transform->rotation.y,
			transform->rotation.z,
			transform->rotation.z
	}, rot);

	glm_mat4_mul(out, rot, out);

	glm_scale(out, (vec3) { transform->scale.scale_x, transform->scale.scale_y, transform->scale.scale_z });
}

void TransformUniformScale(Transform* transform, double scale)
{
	transform->scale.scale_x = scale;
	transform->scale.scale_y = scale;
	transform->scale.scale_z = scale;
}

void TransformGetVersor(Transform* transform, versor out)
{
	out[0] = (float)transform->rotation.x;
	out[1] = (float)transform->rotation.y;
	out[2] = (float)transform->rotation.z;
	out[3] = (float)transform->rotation.w;
}

void TransformSetVersor(Transform* transform, versor rot)
{
	transform->rotation.x = (double)rot[0];
	transform->rotation.y = (double)rot[1];
	transform->rotation.z = (double)rot[2];
	transform->rotation.w = (double)rot[3];
}

void TransformRotate(Transform* transform, vec3 axis, float angle)
{
	versor delta_rot;
	glm_quatv(delta_rot, angle, axis);
	glm_quat_normalize(delta_rot);

	versor current_rot;
	TransformGetVersor(transform, current_rot);
	glm_quat_normalize(current_rot);

	versor result;
	glm_quat_mul(current_rot, delta_rot, result);
	glm_quat_normalize(result);

	TransformSetVersor(transform, result);
}

void TransformGetWorldVersor(ECS* ecs, Entity_t entity, versor out)
{
	Transform* t = MLECSGetComponentTransform(ecs, entity);

	if (t == NULL)
	{
		glm_quat_copy((versor) { 0.0f, 0.0f, 0.0f, 1.0f }, out);
	}

	Parent* p = MLECSGetComponentParent(ecs, entity);

	if (p)
	{
		EntityStatus status = MLEntityManagerGetStatus(ecs->managers.entity_manager, p->parent);

		if (status.alive && (status.generation == p->generation))
		{
			versor parentVersor;
			TransformGetWorldVersor(ecs, p->parent, parentVersor);

			versor localVersor;
			TransformGetVersor(t, localVersor);

			glm_quat_mul(parentVersor, localVersor, out);
			return;
		}	
	}
	
	TransformGetVersor(t, out);
}