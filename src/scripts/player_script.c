#include "MLScript.h"

#include <stdio.h>

#include <stdlib.h>

#include "MLCore.h"

typedef struct PlayerContext
{
	int health;
} PlayerContext;

CREATE_FUNCTION(PlayerScript)
{
	PlayerContext* my_context = calloc(1, sizeof(PlayerContext));	

	*ctx_ptr = my_context;

	printf("pointer is %zu\n", (size_t)*ctx_ptr);
	return;
}

DESTROY_FUNCTION(PlayerScript)
{
	PlayerContext* player_ctx = (PlayerContext*)ctx;

	printf("Destroying player script!\n");

	printf("pointer is %zu\n", (size_t)player_ctx);

	free(player_ctx);

	return;
}

READY_FUNCTION(PlayerScript)
{
	PlayerContext* player_context = (PlayerContext*)ctx;

	player_context->health = 10;

	printf("ready!\n");


	return;
}

UPDATE_FUNCTION(PlayerScript)
{
	PlayerContext* player_ctx = (PlayerContext*)ctx;	
	Input* input = GET_INPUT;
	/*

	if (input->keys[GLFW_KEY_A].just_pressed)
	{
		printf("A pressed!\n");
	}

	if (input->keys[GLFW_KEY_A].is_pressed)
	{
		printf("A down!\n");
	}

	if (input->keys[GLFW_KEY_A].just_released)
	{
		printf("A released!\n");
	}

	if (input->any_key.just_pressed)
	{
		printf("The any key was pressed!\n");
	}

	if (input->any_key.just_released)
	{
		printf("The any key was released!\n");
	}

	*/


	Transform* t = MLECSGetComponentTransform(&game_ctx->ecs, entity);

	const double speed = 1.0f;

	if (input->keys[GLFW_KEY_S].is_pressed)
	{
		t->z += 5 * speed * dt;
	}

	if (input->keys[GLFW_KEY_W].is_pressed)
	{
		t->z -= 5 * speed * dt;
	}

	if (input->keys[GLFW_KEY_A].is_pressed)
	{
		t->x -= speed * dt;
	}

	if (input->keys[GLFW_KEY_D].is_pressed)
	{
		t->x += speed * dt;
	}

	if (input->keys[GLFW_KEY_UP].is_pressed)
	{
		t->y += speed * dt;
	}

	if (input->keys[GLFW_KEY_DOWN].is_pressed)
	{
		t->y -= speed * dt;
	}

	if (input->keys[GLFW_KEY_Q].is_pressed)
	{
		t->ry -= speed * dt;
	}

	if (input->keys[GLFW_KEY_E].is_pressed)
	{
		t->ry += speed * dt;
	}

	if (input->keys[GLFW_KEY_M].is_pressed)
	{
		t->scale += speed * dt;
	}

	if (input->keys[GLFW_KEY_N].is_pressed)
	{
		t->scale -= speed * dt;
	}

	return;
}