#include "scripts/player_script.h"

#include "MLScript.h"

#include <stdio.h>
#include <stdlib.h>

#include <cglm/cglm.h>

#include "MLCore.h"
#include "systems/NameSystem.h"

#include "scripts/teapot_script.h"

DEFAULT_CREATE_FUNCTION(PlayerScript, PlayerContext)

DESTROY_FUNCTION(PlayerScript)
{
	PlayerContext* player_ctx = (PlayerContext*)ctx;

	printf("oh no I died! my health is %d.\n", player_ctx->health);

	return;
}

READY_FUNCTION(PlayerScript)
{
	PlayerContext* player_context = (PlayerContext*)ctx;

	player_context->health = 10;
	player_context->killing_teapot = STI_FALSE;

	printf("ready!\n");

	player_context->teapot = NameSystemFind(game_ctx, "teapot");

	if (player_context->teapot < MAX_ENTITIES)
	{
		printf("Found teapot at entity: {%zu}\n", (size_t)player_context->teapot);
	}
	else
	{
		printf("Couldn't find teapot!\n");
	}

	return;
}

UPDATE_FUNCTION(PlayerScript)
{
	PlayerContext* player_ctx = (PlayerContext*)ctx;	
	Input* input = GET_INPUT;

	if (input->keys[GLFW_KEY_T].just_pressed)
	{
		// attempt to nuke the teapot
		//MLECSDestroyEntity(&game_ctx->ecs, player_ctx->teapot);
		player_ctx->killing_teapot = STI_TRUE;
	}

	if (player_ctx->killing_teapot)
	{
		Script* teapot_script = MLECSGetComponentScript(&game_ctx->ecs, player_ctx->teapot);
		if (teapot_script)
		{
			TeapotCtx* teapot_ctx = (TeapotCtx*)teapot_script->context;

			teapot_ctx->life_force -= dt * 10.0;
		}
	}


	Transform* t = MLECSGetComponentTransform(&game_ctx->ecs, entity);

	vec3 forward_vec = {
	cos(t->rx) * sin(t->ry),
	sin(t->rx),
	cos(t->rx) * cos(t->ry) };
	glm_normalize(forward_vec);

	vec3 world_up = { 0.0f, 1.0f, 0.0f };
	vec3 right_vec;

	glm_vec3_cross(world_up, forward_vec, right_vec);
	glm_normalize(right_vec);
	
	vec3 delta_pos = { 0.0f, 0.0f, 0.0f };

	const float speed = 1.0f * dt;

	if (input->keys[GLFW_KEY_S].is_pressed)
	{
		vec3 delta;
		glm_vec3_scale(forward_vec, -1.0f * speed, delta);
		glm_vec3_add(delta_pos, delta, delta_pos);		
	}

	if (input->keys[GLFW_KEY_W].is_pressed)
	{
		vec3 delta;
		glm_vec3_scale(forward_vec, 1.0f * speed, delta);
		glm_vec3_add(delta_pos, delta, delta_pos);
	}

	if (input->keys[GLFW_KEY_A].is_pressed)
	{
		vec3 delta;
		glm_vec3_scale(right_vec, 1.0f * speed, delta);
		glm_vec3_add(delta_pos, delta, delta_pos);
	}

	if (input->keys[GLFW_KEY_D].is_pressed)
	{
		vec3 delta;
		glm_vec3_scale(right_vec, -1.0f * speed, delta);
		glm_vec3_add(delta_pos, delta, delta_pos);
	}

	if (input->keys[GLFW_KEY_UP].is_pressed)
	{
		vec3 delta;
		glm_vec3_scale(world_up, 1.0f * speed, delta);
		glm_vec3_add(delta_pos, delta, delta_pos);
	}

	if (input->keys[GLFW_KEY_DOWN].is_pressed)
	{
		vec3 delta;
		glm_vec3_scale(world_up, -1.0f * speed, delta);
		glm_vec3_add(delta_pos, delta, delta_pos);
	}

	if (input->keys[GLFW_KEY_Q].is_pressed)
	{
		t->ry -= speed;
	}

	if (input->keys[GLFW_KEY_E].is_pressed)
	{
		t->ry += speed;
	}

	if (input->keys[GLFW_KEY_M].is_pressed)
	{
		t->scale += speed;
	}

	if (input->keys[GLFW_KEY_N].is_pressed)
	{
		t->scale -= speed;
	}

	vec3 pos = {
		(float)t->x,
		(float)t->y,
		(float)t->z
	};

	glm_vec3_add(pos, delta_pos, pos);

	t->x = pos[0];
	t->y = pos[1];
	t->z = pos[2];

	return;
}