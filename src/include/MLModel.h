#pragma once

#include <glad/glad.h>

#include "MLMesh.h"
#include "MLShader.h"

typedef struct Model
{
	Mesh* mesh;		
	ShaderProgram shader_program;
} Model;

Model MLModelCreate(Mesh* mesh, ShaderProgram shader_program);
void MLModelDestroy(Model* model);

void MLModelBind(Model* model);
void MLModelUnbind(Model* model);

void MLModelDraw(Model* model);