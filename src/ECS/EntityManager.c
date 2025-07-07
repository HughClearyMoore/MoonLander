#include "ECS/EntityManager.h"

#include <assert.h>
#include <stdlib.h>

EntityManager* MLEntityManagerCreate()
{
	EntityManager* manager = calloc(1, sizeof(EntityManager));
	assert(manager);	

	manager->free_entities = DynArrayCreate(sizeof(Entity_t), 0, NULL);

	return manager;
}

void MLEntityManagerDestroy(EntityManager* manager)
{
	DynArrayDestroy(&manager->free_entities);

	*manager = (EntityManager){ 0 };

	free(manager);	
}

Entity_t MLEntityManagerNewEntity(EntityManager* manager)
{
	Entity_t new_id = { 0 };

	STI_BOOL is_reusing = STI_FALSE;

	if (DynArraySize(&manager->free_entities))
	{
		is_reusing = STI_TRUE;


		new_id = *(Entity_t*)DynArrayBack(&manager->free_entities);
		DynArrayPopBack(&manager->free_entities);		
	}
	else
	{
		new_id = manager->entity_id_count++;
	}

	assert(new_id < MAX_ENTITIES);

	++manager->entity_count;
	EntityStatus* current_status = &manager->entity_statuses[new_id];
	
	current_status->alive = STI_TRUE;

	if(is_reusing) ++current_status->generation;	

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

EntityStatus MLEntityManagerGetStatus(EntityManager* manager, Entity_t entity)
{
	return manager->entity_statuses[entity];
}