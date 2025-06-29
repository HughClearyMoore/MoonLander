#include "MLScript.h"

#define SCRIPT(name) \
	Script ScriptLink##name() \
	{ \
		Script scr = {0}; \
		scr.ready = &OnReady##name; \
		scr.destroy = &OnDestroy##name; \
		scr.create = &ScriptCreate##name;

#define SCRIPT_HAS_UPDATE(name) \
	scr.update = &OnUpdate##name; \

#define SCRIPT_END \
	return scr; \
}

#include "defs/MLScripts.defs"


#undef SCRIPT
#undef SCRIPT_END

#undef SCRIPT_HAS_UPDATE

