#include "systems/PhysicsSystem.h"

#include <assert.h>
#include <stdio.h>

PhysicsSystem PhysicsSystemCreate(ECS* ecs)
{
	PhysicsSystem system = { 0 };
	Signature_t signature = 0;
	signature |= 1 << ENUM_COMPONENT_Transform;

	system.system = MLECSNewSystem(ecs, signature);

	system.ecs = ecs;

	return system;
}

void PhysicsSystemUpdate(PhysicsSystem* physics_system, double dt)
{
	MLSystem* system = physics_system->system;
	ECS* ecs = physics_system->ecs;

	const size_t sz = DynArraySize(&system->entities);
	Entity_t* entities = system->entities.data;

	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];

		Transform* transform = MLECSGetComponentTransform(ecs, e);
		if (transform == NULL)
		{
			printf("null transform\n");
			continue;
		}
		

		printf("Position: x = '{%f}', y = '{%f}', z = '{%f}'\n",
			transform->x,
			transform->y,
			transform->z);
	}
}