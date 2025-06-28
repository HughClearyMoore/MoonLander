#pragma once

#include <stddef.h>

#include <DynArray.h>

typedef struct Mesh
{
	DynArray vertices;
	size_t vertex_count;
} Mesh;

Mesh MLMeshCreate(size_t vertix_count);
Mesh MLMeshCreateFromCArray(float verts[], size_t count);
void MLMeshDestroy(Mesh* mesh);