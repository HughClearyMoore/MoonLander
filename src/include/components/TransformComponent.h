#pragma once

#include "../ECS/ECSConfig.h"

typedef struct ECS ECS;

typedef struct Transform
{
	double x, y, z;
	double rx, ry, rz;
	double scale;

	double prev_x, prev_y, prev_z;
} Transform;

Transform TransformGetWorldTransform(ECS* ecs, Entity_t entity);
Transform TransformGetInterpolatedWorldTransform(ECS* ecs, Entity_t entity, double alpha);