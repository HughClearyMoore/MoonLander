#include "ECS/SystemManager.h"

#include <assert.h>
#include <stdlib.h>

typedef struct SystemInternal
{
	MLSystem* system;
	Signature_t signature;
} SystemInternal;

typedef struct SystemManager
{
	// DynArray<SystemInternal>	
	DynArray systems;
} SystemManager;

static void SystemDeleter(void* ptr)
{
	SystemInternal* system = (SystemInternal*)ptr;

	MLSystemDestroy(system->system);

	*system = (SystemInternal){ 0 };
}

SystemManager* MLSystemManagerCreate()
{
	SystemManager* manager = calloc(1, sizeof(SystemManager));
	assert(manager);

	manager->systems = DynArrayCreate(sizeof(SystemInternal), 0, &SystemDeleter);

	return manager;
}

void MLSystemManagerDestroy(SystemManager* manager)
{
	DynArrayDestroy(&manager->systems);

	*manager = (SystemManager){ 0 };

	free(manager);
}

MLSystem* MLSystemManagerNewSystem(SystemManager* manager, Signature_t signature)
{
	SystemInternal sys = { 0 };

	sys.system = calloc(1, sizeof(MLSystem));
	assert(sys.system);		

	*sys.system = MLSystemCreate();
	sys.signature = signature;

	DynArrayPush(&manager->systems, &sys);

	return sys.system;
}

void MLSystemManagerEntityDestroyed(SystemManager* manager, Entity_t entity)
{
	SystemInternal* systems = (SystemInternal*)manager->systems.data;

	const size_t system_count = DynArraySize(&manager->systems);

	for (size_t i = 0; i < system_count; ++i)
	{
		MLSystemUntrackEntity(systems[i].system, entity);
	}
}

void MLSystemManagerEntitySignatureChanged(SystemManager* manager, Entity_t entity, Signature_t signature)
{
	SystemInternal* systems = (SystemInternal*)manager->systems.data;
	const size_t system_count = DynArraySize(&manager->systems);

	SystemInternal* system = NULL;

	for (size_t i = 0; i < system_count; ++i)
	{
		system = &systems[i];

		const Signature_t system_signature = system->signature;

		if ((system_signature & signature) == system_signature)
		{
			if (MLSystemIsTrackingEntity(system->system, entity) == STI_FALSE)
			{
				MLSystemTrackEntity(system->system, entity);
			}			
		}
		else
		{
			MLSystemUntrackEntity(system->system, entity);
		}
	}
}