#include "ECS/Component.h"



Name NameComponentCreate(const char* name)
{
	Name name_component = { 0 };

	name_component.name = STIStringCreate(name);

	return name_component;
}

COMPONENT_DESTROY(Name)
{
	Name* name = (Name*)component;

	STIStringDestroy(&name->name);
}




