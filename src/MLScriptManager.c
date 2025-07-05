#include "MLScriptManager.h"

#include <assert.h>
#include <stdlib.h>

#define SCRIPT(name) case SCRIPT_ENUM_##name: return #name;
const char* MLGetScriptName(Script_t script_type)
{
	switch (script_type)
	{
#include "defs/MLScripts.defs"
	}

	return NULL;
}
#undef SCRIPT

static void ScriptDeleter(void* ptr)
{
	MLScript* script = *(MLScript**)ptr;

	free(script);	
}

ScriptManager MLScriptManagerCreate()
{
	ScriptManager manager = { 0 };
	manager.scripts = DynArrayCreate(sizeof(MLScript*), 0, &ScriptDeleter);

	
	// time for witch craft
	// register all the scripts via an index with their enum value
	
	MLScript* scr = NULL;

#define SCRIPT(name) scr = calloc(1, sizeof(MLScript)); \
	assert(scr); \
	*scr = ScriptLink##name(); \
	DynArrayPush(&manager.scripts, &scr);

#include "defs/MLScripts.defs"

#undef SCRIPT

	return manager;
}

void MLScriptManagerDestroy(ScriptManager* manager)
{
	DynArrayDestroy(&manager->scripts);
	*manager = (ScriptManager){ 0 };
}

MLScript* MLScriptManagetGet(ScriptManager* manager, Script_t script_type)
{
	return *(MLScript**)DynArrayGet(&manager->scripts, script_type);
}