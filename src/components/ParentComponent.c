#include "ECS/component.h"

#include <stdio.h>

#include "MLCore.h"

Parent ParentCreate(ECS* ecs, Entity_t entity)
{
	Parent p = { 0 };
	
	EntityStatus status = MLEntityManagerGetStatus(ecs->managers.entity_manager, entity);

	p.parent = entity;
	p.generation = status.generation;

	return p;
}

COMPONENT_DESTROY(Parent)
{
	Parent* parent = (Parent*)component;
	printf("Sir {%zu|%zu}, your child has died!\n", parent->parent, parent->generation);
}
