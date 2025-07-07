#include "scripts/teapot_script.h"

#include "MLScript.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <cglm/cglm.h>

#include "MLCore.h"

static double MakeRandom()
{
	return rand() / (double)RAND_MAX;
}

DEFAULT_CREATE_FUNCTION(TeapotScript, TeapotCtx)

READY_FUNCTION(TeapotScript)
{
	TeapotCtx* teapot = (TeapotCtx*)ctx;
	printf("I, the Teapot, have been born!\n");
	
	teapot->life_force = 100.0;
}

UPDATE_FUNCTION(TeapotScript)
{
	Input* input = GET_INPUT;
	TeapotCtx* teapot = (TeapotCtx*)ctx;

	Transform* t = MLECSGetComponentTransform(&game_ctx->ecs, entity);

	t->scale = teapot->life_force / 100.0;
	t->scale = t->scale < 0.0 ? 0.0 : t->scale;
	
	if (t->scale == 0.0)
	{
		MLECSDestroyEntity(&game_ctx->ecs, entity);
	}

	if (input->keys[GLFW_KEY_LEFT_SHIFT].just_pressed)
	{
		Entity_t new_entity = MLECSNewEntity(&game_ctx->ecs);

		
		vec3 direction = { cos(MakeRandom() * 2 * 3.141592653), sin(MakeRandom() * 2 * 3.141592653), sin(MakeRandom() * 2 * 3.141592653)};
		glm_normalize(direction);

		double distance = 1 + MakeRandom() * 10.0;

		glm_vec3_scale(direction, (float)distance, direction);

		vec3 final_pos = { 0.0, 0.0, 0 };
		glm_vec3_add(final_pos, direction, final_pos);

		Transform trans =
		{
			.x = final_pos[0],
			.y = final_pos[1],
			.z = final_pos[2],
			.scale = MakeRandom() * 3 + 1.0
		};

		MLECSAttachComponentTransform(&game_ctx->ecs, new_entity, &trans);

		Mesh* mesh = MLMeshManagerGetMesh(&game_ctx->mesh_manager, "cube");
		ShaderProgram* program = MLShaderProgramManagerGetProgram(&game_ctx->program_manager, "basic");

		Model model = { .mesh = mesh, .program = program };

		MLECSAttachComponentModel(&game_ctx->ecs, new_entity, &model);
	}
}

DESTROY_FUNCTION(TeapotScript)
{
	printf("I, the Teapot, have been eviscerated!\n");
}