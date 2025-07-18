#include "MLScript.h"

#include "MLCore.h"

#define SCRIPT(name) \
	MLScript ScriptLink##name() \
	{ \
		MLScript scr = {0}; \
		scr.ready = &OnReady##name; \
		scr.destroy = &OnDestroy##name; \
		scr.create = &ScriptCreate##name;

#undef SCRIPT_HAS_UPDATE
#define SCRIPT_HAS_UPDATE(name) \
	scr.update = &OnUpdate##name;

#undef SCRIPT_HAS_COLLISION_ENTER(name)
#define SCRIPT_HAS_COLLISION_ENTER(name) \
	scr.collision_enter = &OnCollisionEnter##name;

#undef SCRIPT_HAS_COLLISION(name)
#define SCRIPT_HAS_COLLISION(name) \
	scr.collision = &OnCollision##name;

#undef SCRIPT_HAS_COLLISION_EXIT(name)
#define SCRIPT_HAS_COLLISION_EXIT(name) \
	scr.collision_exit = &OnCollisionExit##name;


#undef SCRIPT_HAS_FRAME_DRAW(name)
#define SCRIPT_HAS_FRAME_DRAW(name) \
	scr.frame_draw = &OnFrameDraw##name;

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

#undef SCRIPT_HAS_COLLISION_ENTER
#define SCRIPT_HAS_COLLISION_ENTER(name)

#undef SCRIPT_HAS_COLLISION
#define SCRIPT_HAS_COLLISION(name)

#undef SCRIPT_HAS_COLLISION_EXIT
#define SCRIPT_HAS_COLLISION_EXIT(name)

#undef SCRIPT_HAS_FRAME_DRAW
#define SCRIPT_HAS_FRAME_DRAW(name)

