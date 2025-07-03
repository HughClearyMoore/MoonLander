#pragma once

#include <DynArray.h>

#include "System.h"

typedef struct SystemManager
{
	// DynArray<MLSystem*>
	DynArray systems;
} SystemManager;

SystemManager* MLSystemManagerCreate();
void MLSystemManagerDestroy(SystemManager* manager);

MLSystem* MLSystemManagerNewSystem(SystemManager* manager, Signature_t signature);

void MLSystemManagerEntityDestroyed(SystemManager* manager, Entity_t entity);
void MLSystemManagerEntitySignatureChanged(SystemManager* manager, Entity_t entity, Signature_t signature);
