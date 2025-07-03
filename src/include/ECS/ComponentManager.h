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