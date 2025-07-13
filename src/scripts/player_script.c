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
	ECS* ecs = GameECS(game_ctx);

	if (input->keys[GLFW_KEY_T].just_pressed)
	{
		// attempt to nuke the teapot
		//MLECSDestroyEntity(&game_ctx->ecs, player_ctx->teapot);
		player_ctx->killing_teapot = STI_TRUE;
	}

	if (player_ctx->killing_teapot)
	{
		Script* teapot_script = MLECSGetComponentScript(ecs, player_ctx->teapot);
		if (teapot_script)
		{
			TeapotCtx* teapot_ctx = (TeapotCtx*)teapot_script->context;

			teapot_ctx->life_force -= dt * 10.0;
		}
	}


	Transform* t = MLECSGetComponentTransform(ecs, entity);

	versor player_rot;
	TransformGetVersor(t, player_rot);

	vec3 world_up = { 0.0f, 1.0f, 0.0f };
	vec3 forward = { 0.0f, 0.0f, -1.0f };
	vec3 right = { 1.0f, 0.0f, 0.0f };

	vec3 forward_vec = { 0.0f, 0.0f, -1.0f };
	//glm_quat_rotatev(player_rot, forward, forward_vec);
	//glm_normalize(forward_vec);

	vec3 right_vec = { 1.0f, 0.0f, 0.0f };
	//glm_quat_rotatev(player_rot, right, right_vec);
	//glm_normalize(right_vec);

	vec3 forward_move = { 0.0f, 0.0f, -1.0f };
	glm_quat_rotatev(player_rot, forward_move, forward_move);

	vec3 right_move = { 1.0f, 0.0f, 0.0f };
	glm_quat_rotatev(player_rot, right_move, right_move);

	
	vec3 delta_pos = { 0.0f, 0.0f, 0.0f };

	const float speed = 3.0f * dt;

	if (input->keys[GLFW_KEY_S].is_pressed)
	{
		vec3 delta;
		glm_vec3_scale(forward_move, -1.0f * speed, delta);
		glm_vec3_add(delta_pos, delta, delta_pos);		
	}

	if (input->keys[GLFW_KEY_W].is_pressed)
	{
		vec3 delta;
		glm_vec3_scale(forward_move, 1.0f * speed, delta);
		glm_vec3_add(delta_pos, delta, delta_pos);
	}

	if (input->keys[GLFW_KEY_A].is_pressed)
	{
		vec3 delta;
		glm_vec3_scale(right_move, -1.0f * speed, delta);
		glm_vec3_add(delta_pos, delta, delta_pos);
	}

	if (input->keys[GLFW_KEY_D].is_pressed)
	{
		vec3 delta;
		glm_vec3_scale(right_move, 1.0f * speed, delta);
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
		float angle = speed;
		//vec3 axis = { 0.0f, 1.0f, 0.0f };
		TransformRotate(t, world_up, angle);
	}

	if (input->keys[GLFW_KEY_E].is_pressed)
	{
		float angle = -speed;
		//vec3 axis = { 0.0f, 1.0f, 0.0f };
		TransformRotate(t, world_up, angle);		
	}

	if (input->keys[GLFW_KEY_R].is_pressed)
	{
		float angle = speed;
		//vec3 axis = { 0.0f, 1.0f, 0.0f };
		TransformRotate(t, forward, angle);
	}

	if (input->keys[GLFW_KEY_F].is_pressed)
	{
		float angle = -speed;
		//vec3 axis = { 0.0f, 1.0f, 0.0f };
		TransformRotate(t, forward, angle);
	}

	if (input->keys[GLFW_KEY_P].is_pressed)
	{
		float angle = speed;		
		TransformRotate(t, right_vec, angle);
	}

	if (input->keys[GLFW_KEY_L].is_pressed)
	{
		float angle = -speed;		
		TransformRotate(t, right_vec, angle);
	}	

	if (input->keys[GLFW_KEY_N].just_pressed)
	{
		printf("right vector: %.2f, %.2f, %.2f\n",
			right_vec[0], right_vec[1], right_vec[2]);
		versor rot;
		TransformGetVersor(t, rot);

		printf("versor: %.2f, %.2f, %.2f, %.2f\n",
			t->rotation.x, t->rotation.y, t->rotation.z, t->rotation.w);
	}

	vec3 pos = {
		(float)t->position.x,
		(float)t->position.y,
		(float)t->position.z
	};

	glm_vec3_add(pos, delta_pos, pos);

	t->position.x = pos[0];
	t->position.y = pos[1];
	t->position.z = pos[2];

	return;
}