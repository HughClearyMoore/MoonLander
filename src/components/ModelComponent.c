#include "ECS/Component.h"

#include <stdio.h>

COMPONENT_DESTROY(Model)
{
	printf("model destroyed!\n");
}