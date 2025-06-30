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

	printf("Ah I've been hit\n");
	
	--player_ctx->health;

	printf("Health now at %d\n", player_ctx->health);
	

	printf("Calling player update script! %f\n", dt);
	return;
}