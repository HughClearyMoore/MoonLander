#include "MLModel.h"

#include "MLTypes.h"

Model MLModelCreate(Mesh* mesh, GLuint VBO, ShaderProgram shader_program)
{
	Model model = { 0 };
	model.mesh = mesh;
	model.VBO = VBO;
	model.shader_program = shader_program;

	return model;
}

void MLModelDestroy(Model* model)
{	
	*model = (Model){ 0 };
}

void MLModelBind(Model* model)
{
	glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
	glUseProgram(model->shader_program.id);
}

void MLModelUnbind(Model* model)
{
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MLModelDraw(Model* model)
{
	MLModelBind(model);
	glDrawArrays(GL_TRIANGLES, 0, model->mesh->vertex_count);
	MLModelUnbind(model);
}