#pragma once

#include "ECSConfig.h"

#include <DynArray.h>
#include <Utility.h>

typedef struct MLSystem
{
	// DynArray<Entity_t>
	DynArray entities;

	struct
	{
		// DynArray<size_t>
		DynArray entity_to_index;
	} mapping;
} MLSystem;

MLSystem MLSystemCreate();
void MLSystemDestroy(MLSystem* system);

void MLSystemTrackEntity(MLSystem* system, Entity_t entity);
void MLSystemUntrackEntity(MLSystem* system, Entity_t entity);

STI_BOOL MLSystemIsTrackingEntity(MLSystem* system, Entity_t entity);