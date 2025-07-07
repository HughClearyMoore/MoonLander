#pragma once

#include "../ECS/ECSConfig.h"

typedef struct ECS ECS;

typedef struct Parent
{
	Entity_t parent;
	size_t generation;
} Parent;

Parent ParentCreate(ECS* ecs, Entity_t entity);