#include "systems/RenderingSystem.h"

#include <assert.h>
#include <stdlib.h>

RenderingSystem* RenderingSystemCreate(Game* game)
{
	ECS* ecs = &game->ecs;
	RenderingSystem* system = (RenderingSystem*)calloc(1, sizeof(RenderingSystem));

	assert(system);


	system->game = game;
	Signature_t signature = 0;
	signature |= 1 << ENUM_COMPONENT_Transform;
	signature |= 1 << ENUM_COMPONENT_Model;

	system->system = MLECSNewSystem(ecs, signature);

	return system;
}

void RenderingSystemUpdate(RenderingSystem* rendering_system, double dt)
{

	const size_t sz = DynArraySize(&rendering_system->system->entities);

	Entity_t* entities = (Entity_t*)rendering_system->system->entities.data;
	ECS* ecs = &rendering_system->game->ecs;

	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];

		Model* model = MLECSGetComponentModel(ecs, e);

		
		Transform* transform = MLECSGetComponentTransform(ecs, e);


	}
}

void RenderingSystemDestroy(RenderingSystem* rendering_system)
{
	*rendering_system = (RenderingSystem){ 0 };

	free(rendering_system);
}