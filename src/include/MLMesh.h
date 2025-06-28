#pragma once

#include <stddef.h>

#include <glad/glad.h>

#include <DynArray.h>



typedef struct Mesh
{
	DynArray vertices;
	size_t vertex_count;
	size_t vertex_byte_size;
	GLuint VAO;
	GLuint VBO;
} Mesh;

Mesh MLMeshCreate(size_t vertex_count, size_t vertex_byte_size);
Mesh MLMeshCreateFromCArray(float verts[], size_t count, size_t vertex_byte_size);
void MLMeshDestroy(Mesh* mesh);