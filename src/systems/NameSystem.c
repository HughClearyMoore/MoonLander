#include "systems/NameSystem.h"

#include <assert.h>
#include <string.h>

#include "MLCore.h"

NameSystem NameSystemCreate(Game* game)
{
	NameSystem system = { 0 };
	Signature_t signature = 0;
	signature |= 1 << ENUM_COMPONENT_Name;

	system.system = MLECSNewSystem(&game->ecs, signature);

	return system;
}

Entity_t NameSystemFind(Game* game, const char* name)
{
	NameSystem* system = &game->ecs.systems.names;
	Entity_t* entities = (Entity_t*)system->system->entities.data;
	ECS* ecs = &game->ecs;
	const size_t sz = DynArraySize(&system->system->entities);

	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];

		Name* name_component = MLECSGetComponentName(ecs, e);
		assert(name);

		if (strcmp(name_component->name.data, name) == 0)
		{
			return e;
		}
	}

	return MAX_ENTITIES;
}

