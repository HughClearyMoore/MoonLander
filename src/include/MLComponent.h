#pragma once

#include "components/TransformComponent.h"





#define COMPONENT(type) typedef struct type type;

#include "../defs/MLComponents.defs"

#undef COMPONENT

#define COMPONENT(type) void MLComponentDestroy##type(type* component);

#include "../defs/MLComponents.defs"

#undef COMPONENT

#define COMPONENT_DESTROY(type) void MLComponentDestroy##type(type* component)