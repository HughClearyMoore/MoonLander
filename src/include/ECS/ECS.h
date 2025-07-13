#pragma once

#include "ECSConfig.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

#include "systems/RenderingSystem.h"
#include "systems/PhysicsSystem.h"
#include "Systems/ScriptSystem.h"
#include "Systems/NameSystem.h"

#include <DynArray.h>

typedef struct Game Game;

typedef struct ECS
{
	struct
	{
		EntityManager* entity_manager;
		ComponentManager* component_manager;
		SystemManager* system_manager;		
	} managers;

	struct
	{
		RenderingSystem rendering;
		PhysicsSystem physics;
		ScriptSystem scripts;
		NameSystem names;
	} systems;

	struct
	{
		DynArray marked_for_death; // DynArray<Entity_t>
		DynArray marked_for_life; // DynArray<Entity_t>
	} queues;
} ECS;

ECS MLECSCreate();
void MLECSDestroy(ECS* ecs);

Entity_t MLECSNewEntity(ECS* ecs);
void MLECSDestroyEntity(ECS* ecs, Entity_t entity);

void MLECSSetEntitySignature(ECS* ecs, Entity_t entity, Signature_t signature);
Signature_t MLECSGetEntitySignature(ECS* ecs, Entity_t entity);

MLSystem* MLECSNewSystem(ECS* ecs, Signature_t signature);

void MLECSDestroyMarkedEntities(Game* game);
void MLECSReadyMarkedEntities(Game* game);

PhysicsSystem* MLECSPhysicsSystem(Game* game);
EntityManager* MLECSEntityManager(Game* game);

#define COMPONENT(type) type* MLECSGetComponent##type(ECS* ecs, Entity_t entity);

#include "../../defs/MLComponents.defs"

#undef COMPONENT

#define COMPONENT(type) void MLECSAttachComponent##type(ECS* ecs, Entity_t entity, type* component);

#include "../../defs/MLComponents.defs"

#undef COMPONENT

#define COMPONENT(type) void MLECSRemoveComponent##type(ECS* ecs, Entity_t entity);

#include "../../defs/MLComponents.defs"

#undef COMPONENT
