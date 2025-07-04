#include "MLModel.h"

#include <GLFW/glfw3.h>

#include "MLTypes.h"

#include <cglm/cglm.h>

static double angle_count = 0.0;

MLModel MLModelCreate(Mesh* mesh, ShaderProgram* shader_program)
{
	MLModel model = { 0 };
	model.mesh = mesh;
	model.shader_program = shader_program;

	return model;
}

void MLModelDestroy(MLModel* model)
{	
	*model = (MLModel){ 0 };
}

void MLModelBind(MLModel* model)
{
	glBindVertexArray(model->mesh->VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, model->mesh_entry->VBO);
	glUseProgram(model->shader_program->id);
}

void MLModelUnbind(MLModel* model)
{
	glUseProgram(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MLModelDraw(MLModel* model, double dt)
{
	MLModelBind(model);

	const double rot_speed = 1.0f;
	double angle = dt * rot_speed;

	angle_count += angle;

	mat4 mat_model;
	glm_mat4_identity(mat_model);
	glm_rotate(mat_model, angle_count, (vec3) { 1.0f, 1.0f, 1.0f });
	GLuint modelLoc = glGetUniformLocation(model->shader_program->id, "uModel");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*)mat_model);

	glDrawArrays(GL_TRIANGLES, 0, model->mesh->vertex_count);
	MLModelUnbind(model);
}