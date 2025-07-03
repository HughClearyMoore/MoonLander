#pragma once

#include "ECSConfig.h"
#include "EntityManager.h"

typedef struct ECS
{
	struct Managers
	{
		EntityManager entity_manager;
	} managers;
} ECS;

ECS MLECSCreate();
void MLECSDestroy(ECS* ecs);

Entity_t MLECSNewEntity(ECS* ecs);
void MLECSDestroyEntity(ECS* ecs, Entity_t entity);

void MLECSSetEntitySignature(ECS* ecs, Entity_t entity, Signature_t signature);
Signature_t MLECSGetEntitySignature(ECS* ecs, Entity_t entity);