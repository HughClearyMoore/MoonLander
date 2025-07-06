#include "scripts/teapot_script.h"

#include "MLScript.h"

#include <stdio.h>
#include <stdlib.h>

#include "MLCore.h"

DEFAULT_CREATE_FUNCTION(TeapotScript, TeapotCtx)

READY_FUNCTION(TeapotScript)
{
	TeapotCtx* teapot = (TeapotCtx*)ctx;
	printf("I, the Teapot, have been born!\n");
	
	teapot->life_force = 100.0;
}

UPDATE_FUNCTION(TeapotScript)
{
	TeapotCtx* teapot = (TeapotCtx*)ctx;

	Transform* t = MLECSGetComponentTransform(&game_ctx->ecs, entity);

	t->scale = teapot->life_force / 100.0;
	t->scale = t->scale < 0.0 ? 0.0 : t->scale;
	
	if (t->scale == 0.0)
	{
		MLECSDestroyEntity(&game_ctx->ecs, entity);
	}
}

DESTROY_FUNCTION(TeapotScript)
{
	printf("I, the Teapot, have been eviscerated!\n");
}