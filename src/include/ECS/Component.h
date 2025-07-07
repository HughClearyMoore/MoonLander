#pragma once

#include "components/TransformComponent.h"
#include "components/ModelComponent.h"
#include "components/ScriptComponent.h"
#include "components/NameComponent.h"
#include "components/RigidBodyComponent.h"
#include "components/ColliderComponent.h"
#include "components/ParentComponent.h"

#define COMPONENT(type) ENUM_COMPONENT_##type,

typedef enum Component_Enum
{
#include "../defs/MLComponents.defs"
	ENUM_COMPONENT_COUNT
} Component_Enum;

#undef COMPONENT

#define COMPONENT(type) typedef struct type type;

#include "../defs/MLComponents.defs"

#undef COMPONENT

#define COMPONENT(type) void MLComponentDestroy##type( void* component);

#include "../defs/MLComponents.defs"

#undef COMPONENT

#define COMPONENT_DESTROY(type) void MLComponentDestroy##type(void* component)