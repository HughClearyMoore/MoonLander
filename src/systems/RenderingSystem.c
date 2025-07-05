#include "systems/RenderingSystem.h"

#include <stdio.h>

#include <cglm/cglm.h>
#include "MLCore.h"


static void BindModel(Model* model)
{
	glBindVertexArray(model->mesh->VAO);
	glUseProgram(model->program->id);
}

static void UnbindModel()
{
	glUseProgram(0);
	glBindVertexArray(0);
}

static void DrawModel(Model* model, Transform* transform, mat4 viewproj)
{
	mat4 mat_model;
	glm_mat4_identity(mat_model);

	glm_translate(mat_model, (vec3) {
			(float)transform->x,
			(float)transform->y,
			(float)transform->z
	});

	glm_rotate(mat_model, (float)transform->rx, (vec3) { 1.0, 0.0, 0.0 });
	glm_rotate(mat_model, (float)transform->ry, (vec3) { 0.0, 1.0, 0.0 });
	glm_rotate(mat_model, (float)transform->rz, (vec3) { 0.0, 0.0, 1.0 });

	glm_scale_uni(mat_model, (float)transform->scale);

	GLuint modelLoc = glGetUniformLocation(model->program->id, "uModel");
	GLuint viewLoc = glGetUniformLocation(model->program->id, "uViewProj");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*)mat_model);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*)viewproj);

	glDrawArrays(GL_TRIANGLES, 0, model->mesh->vertex_count);
}

static void RenderModel(Model* model, Transform* transform, mat4 viewProj)
{
	BindModel(model);

	DrawModel(model, transform, viewProj);


	UnbindModel();
}

RenderingSystem RenderingSystemCreate(Game* game)
{
	ECS* ecs = &game->ecs;
	RenderingSystem system = { 0 };

	


	system.game = game;
	Signature_t signature = 0;
	signature |= 1 << ENUM_COMPONENT_Transform;
	signature |= 1 << ENUM_COMPONENT_Model;

	system.system = MLECSNewSystem(ecs, signature);

	return system;
}

void RenderingSystemUpdate(Game* game, RenderingSystem* rendering_system, double dt)
{

	const size_t sz = DynArraySize(&rendering_system->system->entities);

	Entity_t* entities = (Entity_t*)rendering_system->system->entities.data;
	ECS* ecs = &rendering_system->game->ecs;

	//

	mat4 proj;
	mat4 view;
	mat4 viewproj;

	// ah
	const size_t width = game->window_width;
	const size_t height = game->window_height;

	float aspect = (float)width / (float)height;

	glm_perspective(glm_rad(60.0f), aspect, 0.1f, 100.0f, proj);

	glm_lookat((vec3){0.0f, 0.0f, 5.0f}, 
		(vec3){0.0f, 0.0f, 0.0f},
		(vec3){0.0f, 1.0f, 0.0f},
		view);

	glm_mat4_mul(proj, view, viewproj);

	//


	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];

		Model* model = MLECSGetComponentModel(ecs, e);

		
		Transform* transform = MLECSGetComponentTransform(ecs, e);

		//printf("Transform: x=%f, y=%f, z=%f\n", transform->x, transform->y, transform->z);
		//printf("Model: ptr=%zu\n", (size_t)model);

		RenderModel(model, transform, viewproj);
	}
}