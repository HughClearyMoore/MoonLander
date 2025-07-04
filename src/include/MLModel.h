#pragma once

#include <glad/glad.h>

#include "MLMesh.h"
#include "MLShader.h"

typedef struct MLModel
{
	Mesh* mesh;		
	ShaderProgram* shader_program;
} MLModel;

MLModel MLModelCreate(Mesh* mesh, ShaderProgram* shader_program);
void MLModelDestroy(MLModel* model);

void MLModelBind(MLModel* model);
void MLModelUnbind(MLModel* model);

void MLModelDraw(MLModel* model, double dt);