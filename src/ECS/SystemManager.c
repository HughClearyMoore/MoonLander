#include "ECS/SystemManager.h"

#include <assert.h>
#include <stdlib.h>

static void SystemDeleter(void* ptr)
{
	MLSystem* system = *(MLSystem**)ptr;

	MLSystemDestroy(system);
}

SystemManager* MLSystemManagerCreate()
{
	SystemManager* manager = calloc(1, sizeof(SystemManager));
	assert(manager);

	manager->systems = DynArrayCreate(sizeof(MLSystem*), 0, &SystemDeleter);

	return manager;
}

void MLSystemManagerDestroy(SystemManager* manager)
{
	DynArrayDestroy(&manager->systems);


	*manager = (SystemManager){ 0 };
}

MLSystem* MLSystemManagerNewSystem(SystemManager* manager, Signature_t signature)
{
	MLSystem* system = calloc(1, sizeof(MLSystem));

	assert(system);

	*system = MLSystemCreate(signature);

	DynArrayPush(&manager->systems, &system);

	return system;
}