#pragma once

#include <stddef.h>

#include <DynArray.h>
#include <Utility.h>

#include "ECSConfig.h"

typedef struct EntityStatus
{
	STI_BOOL alive;
	size_t generation;
} EntityStatus;

typedef struct EntityManager
{
	size_t entity_id_count;
	size_t entity_count;
	// DynArray<Entity_t>
	DynArray free_entities;

	Signature_t signatures[MAX_ENTITIES];
	EntityStatus entity_statuses[MAX_ENTITIES];

} EntityManager;


EntityManager* MLEntityManagerCreate();

Entity_t MLEntityManagerNewEntity(EntityManager* manager);
void MLEntityManagerDestroyEntity(EntityManager* manager, Entity_t entity);

void MLEntityManagerSetSignature(EntityManager* manager, Entity_t entity, Signature_t signature);
Signature_t MLEntityManagerGetSignature(EntityManager* manager, Entity_t entity);

EntityStatus MLEntityManagerGetStatus(EntityManager* manager, Entity_t entity);

void MLEntityManagerDestroy(EntityManager* manager);