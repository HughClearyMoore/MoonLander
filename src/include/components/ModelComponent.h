#pragma once

#include "../MLMesh.h"
#include "../MLShader.h"

typedef struct Model
{
	Mesh* mesh;
	ShaderProgram* program;
	struct
	{
		GLuint model_loc;
		GLuint view_loc;
		GLuint proj_loc;
	} uniform_locations;
} Model;

Model CreateModel(Mesh* mesh, ShaderProgram* program);