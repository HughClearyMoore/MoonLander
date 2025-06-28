#include "MLMeshManager.h"

#define HASH_MAP_BUCKET_SIZE 128

static void MeshDeleter(Mesh* entry)
{
	MLMeshDestroy(entry);
	glDeleteBuffers(1, &entry->VBO);
}

MeshManager MLMeshManagerCreate()
{
	MeshManager manager = { 0 };
	manager.meshes = HashMapCreate(
		HASHMAP_KEY_STRING,
		sizeof(Mesh),
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
		
	glVertexAttribPointer(0, mesh.vertex_byte_size / sizeof(float), GL_FLOAT, GL_FALSE, mesh.vertex_byte_size, (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindVertexArray(0);

	HashMapInsert(&manager->meshes, name, &mesh);
}

Mesh* MLMeshManagerGetMesh(MeshManager* manager, const char* name)
{
	return HashMapGet(&manager->meshes, name);
}