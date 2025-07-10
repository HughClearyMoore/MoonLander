#pragma once

#include <stddef.h>

#define MAX_ENTITIES 1000
#define MAX_COMPONENTS 32

typedef unsigned short Entity_t;
typedef unsigned int Signature_t;

typedef struct Entity_ID_t
{
	Entity_t entity;
	size_t generation;
} Entity_ID_t;
