#pragma once


#define COMPONENT(type) ENUM_COMPONENT_##type,

typedef enum Component_t
{
#include "../defs/MLComponents.defs"
	ENUM_COMPONENT_COUNT
} Component_t;

#undef COMPONENT
