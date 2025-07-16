#include "MLMeshManager.h"

#include <assert.h>
#include <stdlib.h>

#define HASH_MAP_BUCKET_SIZE 128

static void MeshDeleter(void** ptr)
{
	Mesh* mesh = *(Mesh**)ptr;

	MLMeshDestroy(mesh);
	free(mesh);
}

MeshManager MLMeshManagerCreate()
{
	MeshManager manager = { 0 };
	manager.meshes = HashMapCreate(
		HASHMAP_KEY_STRING,
		sizeof(Mesh*),
		HASH_MAP_BUCKET_SIZE,
		&HashMapFNV1AHash,
		&HashMapStringCmp,
		&MeshDeleter
	);

	return manager;
}

void MLMeshManagerDestroy(MeshManager* manager)
{
	HashMapDestroy(&manager->meshes);
}

void MLMeshManagerAddMesh(MeshManager* manager, Mesh mesh, const char* name)
{		
	glGenVertexArrays(1, &mesh.VAO);
	glGenBuffers(1, &mesh.VBO);

	glBindVertexArray(mesh.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count * mesh.vertex_byte_size, mesh.vertices.data, GL_STATIC_DRAW);
		
	// a vertex has 3 parts the vertex (first 3) the normals (next 3) the uvs (next 2)

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, mesh.vertex_byte_size, (void*)0);

	// I think it fills them if it doesn' find them?
	// so
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, mesh.vertex_byte_size, (void*)((3 + 3) * (sizeof(float))));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
	glBindVertexArray(0);

	Mesh* mesh_ptr = calloc(1, sizeof(Mesh));
	assert(mesh_ptr);
	*mesh_ptr = mesh;


	HashMapInsert(&manager->meshes, name, &mesh_ptr);
}

Mesh* MLMeshManagerGetMesh(MeshManager* manager, const char* name)
{
	return *(Mesh**)HashMapGet(&manager->meshes, name);
}