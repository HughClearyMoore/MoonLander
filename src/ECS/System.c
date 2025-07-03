#include "ECS/System.h"

#include <assert.h>

MLSystem MLSystemCreate(const Signature_t signature)
{
	MLSystem system = { 0 };

	system.entities = DynArrayCreate(sizeof(Entity_t), MAX_ENTITIES, NULL);
	system.signature = signature;

	system.mapping.entity_to_index = DynArrayCreate(sizeof(size_t), MAX_ENTITIES, NULL);

	for (size_t i = 0; i < MAX_ENTITIES; ++i)
	{
		size_t invalid_idx = MAX_ENTITIES;

		DynArrayPush(&system.mapping.entity_to_index, &invalid_idx);
	}

	return system;
}
void MLSystemDestroy(MLSystem* system)
{
	DynArrayDestroy(&system->entities);
	DynArrayDestroy(&system->mapping.entity_to_index);

	*system = (MLSystem){ 0 };
}

void MLSystemTrackEntity(MLSystem* system, Entity_t entity)
{
	assert(entity < MAX_ENTITIES);

	const size_t idx = DynArraySize(&system->entities);

	size_t* e_ptr = (size_t*)DynArrayGet(&system->mapping.entity_to_index, (size_t)entity);
	*e_ptr = entity;


	DynArrayPush(&system->entities, &entity);
}

void MLSystemUntrackEntity(MLSystem* system, Entity_t entity)
{
	assert(entity < MAX_ENTITIES);

	size_t* idx = (size_t*)DynArrayGet(&system->mapping.entity_to_index, (size_t)entity);

	if (*idx == MAX_ENTITIES) return;

	size_t last_index = DynArraySize(&system->entities) - 1;
	

	Entity_t back_entity = *(Entity_t*)DynArrayBack(&system->entities);

	// the ol' swap and pop
	DynArraySwap(&system->entities, *idx, last_index, Entity_t);
	DynArrayPopBack(&system->entities);

	size_t* back_idx = (size_t*)DynArrayGet(&system->mapping.entity_to_index, back_entity);
	*back_idx = *idx;

	*idx = MAX_ENTITIES;
}

inline Signature_t MLSystemGetSignature(MLSystem* system)
{
	return system->signature;
}