#include "ECS/Component.h"

#include <stdio.h>

#include "MLCore.h"

COMPONENT_DESTROY(Transform)
{
	Transform* transform = (Transform*)component;

	printf("transform destroyed: %f, %f, %f\n", transform->x, transform->y, transform->z);

	*transform = (Transform){ 0 };
}

Transform TransformGetWorldTransform(ECS* ecs, Entity_t entity)
{
	Transform* t = MLECSGetComponentTransform(ecs, entity);
	if (t == NULL)
	{
		return (Transform) { 0 };
	}

	Parent* p = MLECSGetComponentParent(ecs, entity);

	if (p)
	{
		EntityStatus parent_status = MLEntityManagerGetStatus(ecs->managers.entity_manager, p->parent);

		if ((parent_status.alive == STI_TRUE) && (parent_status.generation == p->generation))
		{
			Transform parent_transform = TransformGetWorldTransform(ecs, p->parent);

			
			Transform world = *t;

			world.x += parent_transform.x;
			world.y += parent_transform.y;
			world.z += parent_transform.z;

			return world;			
		}
	}

	return *t;
}

Transform TransformGetInterpolatedWorldTransform(ECS* ecs, Entity_t entity, double alpha)
{
	Transform* t = MLECSGetComponentTransform(ecs, entity);
	if (t == NULL)
	{
		return (Transform) { 0 };
	}

	Transform result = *t;

	if (MLECSGetComponentRigidBody(ecs, entity))
	{
		result.x = t->prev_x + (t->x - t->prev_x) * alpha;
		result.y = t->prev_y + (t->y - t->prev_y) * alpha;
		result.z = t->prev_z + (t->z - t->prev_z) * alpha;
	}	

	Parent* p = MLECSGetComponentParent(ecs, entity);

	if (p)
	{
		EntityStatus status = MLEntityManagerGetStatus(ecs->managers.entity_manager, p->parent);

		if (status.alive == STI_TRUE && status.generation == p->generation)
		{
			Transform parent_world = TransformGetInterpolatedWorldTransform(ecs, p->parent, alpha);
			result.x += parent_world.x;
			result.y += parent_world.y;
			result.z += parent_world.z;
		}
	}

	return result;
}