#pragma once

#include <cglm/cglm.h>

#include "../ECS/ECSConfig.h"

typedef struct ECS ECS;

typedef struct Transform
{	
	struct
	{
		double x, y, z;
	} position;

	struct
	{
		double x, y, z, w;
	} rotation;

	struct
	{
		double scale_x, scale_y, scale_z;
	} scale;

	struct
	{
		struct
		{
			double prev_x, prev_y, prev_z;
		} position;
		struct
		{
			double prev_x, prev_y, prev_z, prev_w;
		} rotation;		
	} previous;	
} Transform;

void TransformGetWorldTransform(ECS* ecs, Entity_t entity, mat4 out);
void TransformGetInterpolatedWorldTransform(ECS* ecs, Entity_t entity, double alpha, mat4 out);

Transform TransformIdentity();
void TransformToMatrix(const Transform* transform, mat4 out);

void TransformUniformScale(Transform* transform, double scale);
void TransformGetVersor(Transform* transform, versor out);
void TransformSetVersor(Transform* transform, versor rot);

void TransformRotate(Transform* transform, vec3 axis, float angle);