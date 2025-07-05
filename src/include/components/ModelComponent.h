#pragma once

#include "../MLMesh.h"
#include "../MLShader.h"

typedef struct Model
{
	Mesh* mesh;
	ShaderProgram* program;
} Model;