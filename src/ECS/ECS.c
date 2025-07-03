#include "ECS/ECS.h"

ECS MLECSCreate()
{
	ECS ecs = { 0 };

	ecs.managers.entity_manager = MLEntityManagerCreate();

	return ecs;
}

void MLECSDestroy(ECS* ecs)
{
	MLEntityManagerDestroy(&ecs->managers.entity_manager);

	*ecs = (ECS){ 0 };
}

Entity_t MLECSNewEntity(ECS* ecs)
{
	return MLEntityManagerNewEntity(&ecs->managers.entity_manager);
}

void MLECSDestroyEntity(ECS* ecs, Entity_t entity)
{
	MLEntityManagerDestroyEntity(&ecs->managers.entity_manager, entity);
}

void MLECSSetEntitySignature(ECS* ecs, Entity_t entity, Signature_t signature)
{
	MLEntityManagerSetSignature(&ecs->managers.entity_manager, entity, signature);
}
Signature_t MLECSGetEntitySignature(ECS* ecs, Entity_t entity)
{
	return MLEntityManagerGetSignature(&ecs->managers.entity_manager, entity);
}