#include "MLScriptManager.h"

#include <assert.h>
#include <stdlib.h>

#include "MLScript.h"

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

static void ScriptDeleter(void** ptr)
{
	Script* script = *(Script**)ptr;

	free(script);	
}

ScriptManager MLScriptManagerCreate()
{
	ScriptManager manager = { 0 };
	manager.scripts = DynArrayCreate(sizeof(Script*), 0, &ScriptDeleter);

	
	// time for witch craft
	// register all the scripts via an index with their enum value
	
	Script* scr = NULL;

#define SCRIPT(name) scr = calloc(1, sizeof(Script)); \
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

Script* MLScriptManagetGet(ScriptManager* manager, Script_t script_type)
{
	return *(Script**)DynArrayGet(&manager->scripts, script_type);
}