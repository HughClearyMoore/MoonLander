#include "ECS/Component.h"

#include <stdio.h>

#include <GLFW/glfw3.h>

COMPONENT_DESTROY(Model)
{
	printf("model destroyed!\n");
}

Model CreateModel(Mesh* mesh, ShaderProgram* program)
{
	Model model = { 0 };

	model.mesh = mesh;
	model.program = program;

	model.uniform_locations.model_loc = glGetUniformLocation(program->id, "uModel");
	model.uniform_locations.view_loc = glGetUniformLocation(program->id, "uView");
	model.uniform_locations.proj_loc = glGetUniformLocation(program->id, "uProj");


	return model;
}