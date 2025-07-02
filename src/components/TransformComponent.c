#include "MLComponent.h"

#include <stdio.h>

COMPONENT_DESTROY(Transform)
{
	printf("%f, %f, %f\n", component->x, component->y, component->z);

	*component = (Transform){ 0 };
}

