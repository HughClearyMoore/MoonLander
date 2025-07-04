#pragma once

#include "ComponentArray.h"

typedef struct ComponentManager
{
	struct Components
	{
		ComponentArray array[ENUM_COMPONENT_COUNT];
	} Components;

} ComponentManager;

ComponentManager* MLComponentManagerCreate();
void MLComponentManagerDestroy(ComponentManager* manager);

void MLComponentManagerEntityDestroyed(ComponentManager* manager, Entity_t entity);