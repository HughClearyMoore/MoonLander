#include "systems/PhysicsSystem.h"

#include <assert.h>
#include <stdio.h>

#include "MLCore.h"
#include "ECS/Component.h"

PhysicsSystem PhysicsSystemCreate(Game* game)
{
	PhysicsSystem system = { 0 };
	Signature_t signature = 0;
	signature |= 1 << ENUM_COMPONENT_Transform;

	system.system = MLECSNewSystem(&game->ecs, signature);

	system.game = game;

	return system;
}

void PhysicsSystemUpdate(PhysicsSystem* physics_system, double dt)
{
	MLSystem* system = physics_system->system;
	ECS* ecs = &physics_system->game->ecs;

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