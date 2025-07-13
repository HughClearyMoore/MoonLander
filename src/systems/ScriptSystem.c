#include "systems/ScriptSystem.h"

#include "MLCore.h"

ScriptSystem ScriptSystemCreate(Game* game, ECS* ecs)
{
	ScriptSystem system = { 0 };
	Signature_t signature = 0;
	signature |= 1 << ENUM_COMPONENT_Script;

	system.system = MLECSNewSystem(ecs, signature);

	return system;
}

void ScriptSystemUpdate(Game* game, ScriptSystem* system, double dt)
{
	Entity_t* entities = (Entity_t*)system->system->entities.data;

	const size_t sz = DynArraySize(&system->system->entities);


	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];

		Script* script = MLECSGetComponentScript(GameECS(game), e);

		if (script->script->update)
		{
			script->script->update(game, e, script->context, dt);
		}
	}
}

void ScriptSystemDestroy(Game* game, ScriptSystem* system)
{
	Entity_t* entities = (Entity_t*)system->system->entities.data;

	const size_t sz = DynArraySize(&system->system->entities);


	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];

		Script* script = MLECSGetComponentScript(GameECS(game), e);

		if (script->script->destroy)
		{
			script->script->destroy(game, e, script->context);
		}
	}
}