#pragma once

#include "ECSConfig.h"
#include "EntityManager.h"
#include "ComponentManager.h"

typedef struct ECS
{
	struct Managers
	{
		EntityManager entity_manager;
		ComponentManager component_manager;
	} managers;
} ECS;

ECS MLECSCreate();
void MLECSDestroy(ECS* ecs);

Entity_t MLECSNewEntity(ECS* ecs);
void MLECSDestroyEntity(ECS* ecs, Entity_t entity);

void MLECSSetEntitySignature(ECS* ecs, Entity_t entity, Signature_t signature);
Signature_t MLECSGetEntitySignature(ECS* ecs, Entity_t entity);

#define COMPONENT(type) type* MLECSGetComponent##type(ECS* ecs, Entity_t entity);

#include "../../defs/MLComponents.defs"

#undef COMPONENT

#define COMPONENT(type) void MLECSAttachComponent##type(ECS* ecs, Entity_t entity, type* component);

#include "../../defs/MLComponents.defs"

#undef COMPONENT

#define COMPONENT(type) void MLECSRemoveComponent##type(ECS* ecs, Entity_t entity);

#include "../../defs/MLComponents.defs"

#undef COMPONENT
