#include "MLScript.h"

#include "MLCore.h"

#define SCRIPT(name) \
	Script ScriptLink##name() \
	{ \
		Script scr = {0}; \
		scr.ready = &OnReady##name; \
		scr.destroy = &OnDestroy##name; \
		scr.create = &ScriptCreate##name;

#undef SCRIPT_HAS_UPDATE
#define SCRIPT_HAS_UPDATE(name) \
	scr.update = &OnUpdate##name; \

#undef SCRIPT_END
#define SCRIPT_END \
	return scr; \
}

#include "defs/MLScripts.defs"


#undef SCRIPT

#undef SCRIPT_END
#define SCRIPT_END

#undef SCRIPT_HAS_UPDATE
#define SCRIPT_HAS_UPDATE(name)

