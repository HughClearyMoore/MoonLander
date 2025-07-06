#pragma once

#include "ECS/ECSConfig.h"

#include <Utility.h>

typedef struct PlayerContext
{
	int health;
	Entity_t teapot;
	STI_BOOL killing_teapot;

} PlayerContext;