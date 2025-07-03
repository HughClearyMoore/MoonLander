#include "ECS/EntityManager.h"

#include <assert.h>

EntityManager MLEntityManagerCreate()
{
	EntityManager manager = { 0 };

	manager.free_entities = DynArrayCreate(sizeof(Entity_t), 0, NULL);

	return manager;
}

void MLEntityManagerDestroy(EntityManager* manager)
{
	DynArrayDestroy(&manager->free_entities);


	*manager = (EntityManager){ 0 };
}

Entity_t MLEntityManagerNewEntity(EntityManager* manager)
{
	Entity_t new_id = { 0 };

	if (DynArraySize(&manager->free_entities))
	{
		size_t size = DynArraySize(&manager->free_entities);


		new_id = *(Entity_t*)DynArrayBack(&manager->free_entities);
		DynArrayPopBack(&manager->free_entities);		
	}
	else
	{
		new_id = manager->entity_id_count++;
	}

	assert(new_id < MAX_ENTITIES);

	++manager->entity_count;

	return new_id;
}

void MLEntityManagerDestroyEntity(EntityManager* manager, Entity_t entity)
{
	assert(manager->entity_count);

	manager->signatures[entity] = (Signature_t){ 0 };

	DynArray* queue = &manager->free_entities;
	DynArrayPush(queue, &entity);

	--manager->entity_count;
}

void MLEntityManagerSetSignature(EntityManager* manager, Entity_t entity, Signature_t signature)
{
	assert(entity < MAX_ENTITIES);

	manager->signatures[entity] = signature;
}

Signature_t MLEntityManagerGetSignature(EntityManager* manager, Entity_t entity)
{
	assert(entity < MAX_ENTITIES);

	return manager->signatures[entity];
}