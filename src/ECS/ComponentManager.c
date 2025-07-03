#include "ECS/ComponentManager.h"

#include <assert.h>
#include <stdlib.h>

ComponentManager* MLComponentManagerCreate()
{
	ComponentManager* manager = calloc(1, sizeof(ComponentManager));
	assert(manager);

#define COMPONENT(type) manager->Components.array[ENUM_COMPONENT_##type] = MLComponentArrayCreate(sizeof(type), &MLComponentDestroy##type);	

#include "../defs/MLComponents.defs"

#undef COMPONENT


	return manager;
}

void MLComponentManagerDestroy(ComponentManager* manager)
{
	for (size_t i = 0; i < ENUM_COMPONENT_COUNT; ++i)
	{
		MLComponentArrayDestroy(&manager->Components.array[i]);
	}

	*manager = (ComponentManager){ 0 };
}