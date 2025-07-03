#pragma once

#include <DynArray.h>

#include "Component.h"
#include "ECSConfig.h"

typedef struct ComponentArray ComponentArray;

typedef void(*ComponentDestroy_t)(void* component);

typedef struct ComponentArray
{
	struct Data
	{
		// DynArray<Component>
		DynArray components;
		size_t size;
	} data;

	struct Mapping
	{
		size_t entity_to_index[MAX_ENTITIES];
		Entity_t index_to_entity[MAX_ENTITIES];
	} mapping;
	
	ComponentDestroy_t component_destroy;
} ComponentArray;

ComponentArray MLComponentArrayCreate(size_t component_size_in_bytes, ComponentDestroy_t component_destroy);
void MLComponentArrayDestroy(ComponentArray* array);

void MLComponentArrayAttachComponent(ComponentArray* array, Entity_t entity, void* component);
void MLComponentArrayRemoveComponent(ComponentArray* array, Entity_t entity);

void* MLComponentArrayGetComponent(ComponentArray* array, Entity_t entity);

void MLComponentArrayEntityDestroyed(ComponentArray* array, Entity_t entity);