#pragma once

#include <Utility.h>

#include "../MLMesh.h"
#include "../MLShader.h"
#include "../MLTextureManager.h"

typedef struct Game Game;

typedef struct Material3D
{
	Mesh* mesh;
	ShaderProgram* program;

	struct
	{
		GLuint model_loc;
		GLuint view_loc;
		GLuint proj_loc;
		GLuint texture_loc;
	} uniforms;	
} Material3D;

typedef struct Model
{
	struct
	{
		MLTexture* texture;
		STI_BOOL has_texture;
	} texture;
	

	union
	{
		Material3D material_3d;		
	} material;

	
} Model;

Model CreateModel3D(Mesh* mesh, ShaderProgram* program, MLTexture* texture);
Model CreateModel3DHelper(Game* game, const char* mesh_name, const char* program_name, const char* texture_name);