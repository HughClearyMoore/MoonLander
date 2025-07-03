#include "ECS/Component.h"

#include <stdio.h>

COMPONENT_DESTROY(Transform)
{
	Transform* transform = (Transform*)component;

	printf("%f, %f, %f\n", transform->x, transform->y, transform->z);

	*transform = (Transform){ 0 };
}

