#include "MLMesh.h"

#include <assert.h>

#include "MLTypes.h"

Mesh MLMeshCreateFromCArray(float verts[], size_t count)
{
	Mesh mesh = MLMeshCreate(count);

	DynArrayBatchPush(&mesh.vertices, verts, count, 0);	

	return mesh;
}

Mesh MLMeshCreate(size_t vertex_count)
{
	Mesh mesh = { 0 };
	mesh.vertex_count = vertex_count;

	mesh.vertices = DynArrayCreate(sizeof(Vertex), vertex_count, NULL);

	return mesh;
}

void MLMeshDestroy(Mesh* mesh)
{
	assert(mesh && "Tried to delete null mesh");
	DynArrayDestroy(&mesh->vertices);
}