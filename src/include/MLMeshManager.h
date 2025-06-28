#pragma once

#include <glad/glad.h>

#include <HashMap.h>

#include "MLMesh.h"

typedef struct MeshManager
{
	HashMap meshes;
} MeshManager;

MeshManager MLMeshManagerCreate();
void MLMeshManagerDestroy(MeshManager* manager);

// note: takes ownership of mesh
void MLMeshManagerAddMesh(MeshManager* manager, Mesh mesh, const char* name);

Mesh* MLMeshManagerGetMesh(MeshManager* manager, const char* name);