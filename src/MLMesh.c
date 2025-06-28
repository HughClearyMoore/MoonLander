#include "MLMesh.h"

#include <assert.h>

#include "MLTypes.h"

Mesh MLMeshCreateFromCArray(float verts[], size_t count, size_t vertex_byte_size)
{
	Mesh mesh = MLMeshCreate(count, vertex_byte_size);

	DynArrayBatchPush(&mesh.vertices, verts, count, 0);	

	return mesh;
}

Mesh MLMeshCreate(size_t vertex_count, size_t vertex_byte_size)
{
	Mesh mesh = { 0 };
	mesh.vertex_count = vertex_count;
	mesh.vertex_byte_size = vertex_byte_size;

	mesh.vertices = DynArrayCreate(vertex_byte_size, vertex_count, NULL);

	return mesh;
}

void MLMeshDestroy(Mesh* mesh)
{
	assert(mesh && "Tried to delete null mesh");
	DynArrayDestroy(&mesh->vertices);
}