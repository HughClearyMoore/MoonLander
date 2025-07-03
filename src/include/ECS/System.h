#pragma once

#include "ECSConfig.h"

#include <DynArray.h>

typedef struct MLSystem
{
	// DynArray<Entity_t>
	DynArray entities;
	Signature_t signature;

	struct
	{
		// DynArray<size_t>
		DynArray entity_to_index;
	} mapping;
} MLSystem;

MLSystem MLSystemCreate(const Signature_t signature);
void MLSystemDestroy(MLSystem* system);

void MLSystemTrackEntity(MLSystem* system, Entity_t entity);
void MLSystemUntrackEntity(MLSystem* system, Entity_t entity);

inline Signature_t MLSystemGetSignature(MLSystem* system);