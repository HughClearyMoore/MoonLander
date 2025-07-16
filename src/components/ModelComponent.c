#include "ECS/Component.h"

#include <stdio.h>
#include <GLFW/glfw3.h>

#include "MLCore.h"

COMPONENT_DESTROY(Model)
{
	printf("model destroyed!\n");
}

Model CreateModel3D(Mesh* mesh, ShaderProgram* program, MLTexture* texture)
{
	Model model = { 0 };

	model.material.material_3d.mesh = mesh;
	model.material.material_3d.program = program;

	model.material.material_3d.uniforms.model_loc = glGetUniformLocation(program->id, "uModel");
	model.material.material_3d.uniforms.view_loc = glGetUniformLocation(program->id, "uView");
	model.material.material_3d.uniforms.proj_loc = glGetUniformLocation(program->id, "uProj");
	
	if (texture)
	{
		model.texture.has_texture = STI_TRUE;
		model.texture.texture = texture;

		model.material.material_3d.uniforms.texture_loc = glGetUniformLocation(program->id, "uTexture");
	}


	return model;
}

Model CreateModel3DHelper(Game* game, const char* mesh_name, const char* program_name, const char* texture_name)
{
	MeshManager* mesh_manager = &game->mesh_manager;
	ShaderProgramManager* program_manager = &game->program_manager;
	MLTextureManager* texture_manager = &game->managers.texture_manager;

	Mesh* mesh = MLMeshManagerGetMesh(mesh_manager, mesh_name);

	assert(mesh);

	ShaderProgram* program = MLShaderProgramManagerGetProgram(program_manager, program_name);

	assert(program);

	MLTexture* texture = NULL;

	if (texture_name)
	{
		texture = MLTextureManagerGet(texture_manager, texture_name);
		assert(texture);
	}

	Model model = CreateModel3D(mesh, program, texture);

	return model;
}