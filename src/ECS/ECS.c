#include "ECS/ECS.h"

ECS MLECSCreate()
{
	ECS ecs = { 0 };

	ecs.managers.entity_manager = MLEntityManagerCreate();
	ecs.managers.component_manager = MLComponentManagerCreate();
	ecs.managers.system_manager = MLSystemManagerCreate();

	return ecs;
}

void MLECSDestroy(ECS* ecs)
{
	MLEntityManagerDestroy(ecs->managers.entity_manager);
	MLComponentManagerDestroy(ecs->managers.component_manager);	
	MLSystemManagerDestroy(ecs->managers.system_manager);

	*ecs = (ECS){ 0 };
}

Entity_t MLECSNewEntity(ECS* ecs)
{
	return MLEntityManagerNewEntity(ecs->managers.entity_manager);
}

void MLECSDestroyEntity(ECS* ecs, Entity_t entity)
{
	MLEntityManagerDestroyEntity(ecs->managers.entity_manager, entity);
	MLComponentManagerEntityDestroyed(ecs->managers.component_manager, entity);
	MLSystemManagerEntityDestroyed(ecs->managers.system_manager, entity);
}

void MLECSSetEntitySignature(ECS* ecs, Entity_t entity, Signature_t signature)
{
	MLEntityManagerSetSignature(ecs->managers.entity_manager, entity, signature);
}
Signature_t MLECSGetEntitySignature(ECS* ecs, Entity_t entity)
{
	return MLEntityManagerGetSignature(ecs->managers.entity_manager, entity);
}

MLSystem* MLECSNewSystem(ECS* ecs, Signature_t signature)
{
	MLSystem* system = MLSystemManagerNewSystem(ecs->managers.system_manager, signature);

	return system;
}

// I NEED TO DO SIGNATURES IN THESE FUNCTIONS (IT WONT WORK IF YOU DON'T HUGH)

#define COMPONENT(type) type* MLECSGetComponent##type(ECS* ecs, Entity_t entity) \
{ \
	ComponentArray* array = &ecs->managers.component_manager->Components.array[ENUM_COMPONENT_##type]; \
	return (type*)MLComponentArrayGetComponent(array, entity); \
}
#include "../defs/MLComponents.defs"

#undef COMPONENT

#define COMPONENT(type) void MLECSAttachComponent##type(ECS* ecs, Entity_t entity, type* component) \
{ \
	ComponentArray* array = &ecs->managers.component_manager->Components.array[ENUM_COMPONENT_##type]; \
	MLComponentArrayAttachComponent(array, entity, component); \
	Signature_t signature = MLECSGetEntitySignature(ecs, entity); \
	signature |= 1 << ENUM_COMPONENT_##type; \
	MLECSSetEntitySignature(ecs, entity, signature); \
	MLSystemManagerEntitySignatureChanged(ecs->managers.system_manager, entity, signature); \
} 
#include "../defs/MLComponents.defs"

#undef COMPONENT


#define COMPONENT(type) void MLECSRemoveComponent##type(ECS* ecs, Entity_t entity) \
{ \
	ComponentArray* array = &ecs->managers.component_manager->Components.array[ENUM_COMPONENT_##type]; \
	MLComponentArrayRemoveComponent(array, entity); \
	Signature_t signature = MLECSGetEntitySignature(ecs, entity); \
	const Signature_t temp = 1; \
	signature &= ~(temp << ENUM_COMPONENT_##type); \
	MLECSSetEntitySignature(ecs, entity, signature); \
	MLSystemManagerEntitySignatureChanged(ecs->managers.system_manager, entity, signature); \
} 
#include "../defs/MLComponents.defs"

#undef COMPONENT