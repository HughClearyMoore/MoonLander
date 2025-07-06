#include "ECS/Component.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "MLCore.h"

Script ScriptComponentCreate(Game* game, Script_t type)
{
	ScriptManager* manager = &game->script_manager;
	assert(manager);

	Script script = { 0 };

	script.script = MLScriptManagetGet(manager, type);

	assert(script.script);

	script.script->create(game, &script.context);

	return script;
}

COMPONENT_DESTROY(Script)
{
	Script* script = (Script*)component;

	printf("Freeing script data at: {%p}\n", script->context);

	free(script->context);
}