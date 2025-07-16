#include "ECS/ECS.h"

#include "MLCore.h"

ECS MLECSCreate()
{
	ECS ecs = { 0 };

	ecs.managers.entity_manager = MLEntityManagerCreate();
	ecs.managers.component_manager = MLComponentManagerCreate();
	ecs.managers.system_manager = MLSystemManagerCreate();	

	ecs.queues.marked_for_death = DynArrayCreate(sizeof(Entity_t), MAX_ENTITIES, NULL);
	ecs.queues.marked_for_life = DynArrayCreate(sizeof(Entity_t), MAX_ENTITIES, NULL);

	return ecs;
}

void MLECSDestroy(ECS* ecs)
{
	MLEntityManagerDestroy(ecs->managers.entity_manager);
	MLComponentManagerDestroy(ecs->managers.component_manager);	
	MLSystemManagerDestroy(ecs->managers.system_manager);

	DynArrayDestroy(&ecs->queues.marked_for_death);
	DynArrayDestroy(&ecs->queues.marked_for_life);

	*ecs = (ECS){ 0 };
}

Entity_t MLECSNewEntity(ECS* ecs)
{
	Entity_t e = MLEntityManagerNewEntity(ecs->managers.entity_manager);
	DynArrayPush(&ecs->queues.marked_for_life, &e);

	printf("constructed new entity %d\n", e);

	return e;
}

void MLECSDestroyEntity(ECS* ecs, Entity_t entity)
{
	// defer destruction until the end of the frame

	// mark the entity for death
	DynArrayPush(&ecs->queues.marked_for_death, &entity);

	/*
	MLEntityManagerDestroyEntity(ecs->managers.entity_manager, entity);
	MLComponentManagerEntityDestroyed(ecs->managers.component_manager, entity);
	MLSystemManagerEntityDestroyed(ecs->managers.system_manager, entity);
	*/
}

void MLECSSetEntitySignature(ECS* ecs, Entity_t entity, Signature_t signature)
{
	MLEntityManagerSetSignature(ecs->managers.entity_manager, entity, signature);
}
Signature_t MLECSGetEntitySignature(ECS* ecs, Entity_t entity)
{
	return MLEntityManagerGetSignature(ecs->managers.entity_manager, entity);
}

MLSystem* MLECSNewSystem(ECS* ecs, Signature_t signature)
{
	MLSystem* system = MLSystemManagerNewSystem(ecs->managers.system_manager, signature);

	return system;
}

// I NEED TO DO SIGNATURES IN THESE FUNCTIONS (IT WONT WORK IF YOU DON'T HUGH)

#define COMPONENT(type) type* MLECSGetComponent##type(ECS* ecs, Entity_t entity) \
{ \
	ComponentArray* array = &ecs->managers.component_manager->Components.array[ENUM_COMPONENT_##type]; \
	return (type*)MLComponentArrayGetComponent(array, entity); \
}
#include "../defs/MLComponents.defs"

#undef COMPONENT

#define COMPONENT(type) void MLECSAttachComponent##type(ECS* ecs, Entity_t entity, type* component) \
{ \
	ComponentArray* array = &ecs->managers.component_manager->Components.array[ENUM_COMPONENT_##type]; \
	MLComponentArrayAttachComponent(array, entity, component); \
	Signature_t signature = MLECSGetEntitySignature(ecs, entity); \
	signature |= 1 << ENUM_COMPONENT_##type; \
	MLECSSetEntitySignature(ecs, entity, signature); \
	MLSystemManagerEntitySignatureChanged(ecs->managers.system_manager, entity, signature); \
} 
#include "../defs/MLComponents.defs"

#undef COMPONENT


#define COMPONENT(type) void MLECSRemoveComponent##type(ECS* ecs, Entity_t entity) \
{ \
	ComponentArray* array = &ecs->managers.component_manager->Components.array[ENUM_COMPONENT_##type]; \
	MLComponentArrayRemoveComponent(array, entity); \
	Signature_t signature = MLECSGetEntitySignature(ecs, entity); \
	const Signature_t temp = 1; \
	signature &= ~(temp << ENUM_COMPONENT_##type); \
	MLECSSetEntitySignature(ecs, entity, signature); \
	MLSystemManagerEntitySignatureChanged(ecs->managers.system_manager, entity, signature); \
} 
#include "../defs/MLComponents.defs"

#undef COMPONENT

void MLECSDestroyMarkedEntities(Game* game)
{
	ECS* ecs = GameECS(game);
	DynArray* marked_for_death = &ecs->queues.marked_for_death;	

	const size_t sz = DynArraySize(marked_for_death);

	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = *(Entity_t*)DynArrayGet(marked_for_death, i);

		Script* script = MLECSGetComponentScript(ecs, e);

		if (script && script->script && script->script->destroy)
		{
			script->script->destroy(game, e, script->context);			
		}

		ecs->managers.entity_manager->entity_statuses[e].alive = STI_FALSE;

		printf("Destroyed entity: %d\n", e);
	}


	while (DynArraySize(marked_for_death))
	{
		Entity_t e = *(Entity_t*)DynArrayBack(marked_for_death);

		MLEntityManagerDestroyEntity(ecs->managers.entity_manager, e);
		MLComponentManagerEntityDestroyed(ecs->managers.component_manager, e);
		MLSystemManagerEntityDestroyed(ecs->managers.system_manager, e);

		DynArrayPopBack(marked_for_death);
	}
}

void MLECSReadyMarkedEntities(Game* game)
{
	ECS* ecs = GameECS(game);
	DynArray* marked = &ecs->queues.marked_for_life;	

	while (DynArraySize(marked))
	{
		Entity_t e = *(Entity_t*)DynArrayBack(marked);

		Script* script = MLECSGetComponentScript(ecs, e);

		if (script && script->script && script->script->ready)
		{
			script->script->ready(game, e, script->context);
		}
		

		DynArrayPopBack(marked);
	}
}

PhysicsSystem* MLECSPhysicsSystem(Game* game)
{
	return &GameECS(game)->systems.physics;	
}

EntityManager* MLECSEntityManager(Game* game)
{
	return GameECS(game)->managers.entity_manager;	
}