#include "ECS/ComponentArray.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

ComponentArray MLComponentArrayCreate(size_t component_size_in_bytes, ComponentDestroy_t component_destroy)
{
	ComponentArray arr = { 0 };

	arr.data.components = DynArrayCreate(component_size_in_bytes, MAX_ENTITIES, NULL);
	arr.component_destroy = component_destroy;

	for (size_t i = 0; i < MAX_ENTITIES; ++i)
	{
		arr.mapping.entity_to_index[i] = MAX_ENTITIES;
		arr.mapping.index_to_entity[i] = MAX_ENTITIES;
	}

	return arr;
}

void MLComponentArrayDestroy(ComponentArray* array)
{
	const size_t sz = array->data.size;
	const ComponentDestroy_t component_destroy = array->component_destroy;

	for (size_t i = 0; i < sz; ++i)
	{
		void* component = DynArrayGet(&array->data.components, i);
		component_destroy(component);		
	}	

	DynArrayDestroy(&array->data.components);

	*array = (ComponentArray){ 0 };
}

void MLComponentArrayAttachComponent(ComponentArray* array, Entity_t entity, void* component)
{
	const size_t idx = array->data.size++;

	array->mapping.entity_to_index[entity] = idx;
	array->mapping.index_to_entity[idx] = entity;

	DynArrayPush(&array->data.components, component);
}

void MLComponentArrayRemoveComponent(ComponentArray* array, Entity_t entity)
{
	const size_t idx = array->mapping.entity_to_index[entity];
	assert(idx != MAX_ENTITIES);
	
	const size_t back_idx = DynArraySize(&array->data.components) - 1;

	void* back_component = DynArrayBack(&array->data.components);
	void* component = DynArrayGet(&array->data.components, idx);

	const Entity_t back_entity = array->mapping.index_to_entity[back_idx];

	array->component_destroy(component);

	if (component != back_component)
	{
		const size_t element_size = array->data.components.element_size;
		memcpy(component, back_component, element_size);
	}	

	DynArrayPopBack(&array->data.components);
	
	if (idx != back_idx)
	{
		array->mapping.index_to_entity[idx] = back_entity;
		array->mapping.entity_to_index[back_entity] = idx;		
	}

	array->mapping.entity_to_index[entity] = MAX_ENTITIES;
	array->mapping.index_to_entity[back_idx] = MAX_ENTITIES;
}

void* MLComponentArrayGetComponent(ComponentArray* array, Entity_t entity)
{
	const size_t idx = array->mapping.entity_to_index[entity];

	if (idx == MAX_ENTITIES) return NULL;

	return DynArrayGet(&array->data.components, idx);
}

void MLComponentArrayEntityDestroyed(ComponentArray* array, Entity_t entity)
{
	const size_t idx = array->mapping.entity_to_index[entity];

	if (idx == MAX_ENTITIES) return;

	MLComponentArrayRemoveComponent(array, entity);
}