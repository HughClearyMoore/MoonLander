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

static void DrawModel(Model* model, mat4 transform, mat4 view, mat4 proj)
{
	
	glUniformMatrix4fv(model->uniform_locations.model_loc, 1, GL_FALSE,
		(const GLfloat*)transform);

	glUniformMatrix4fv(model->uniform_locations.view_loc, 1, GL_FALSE,
		(const GLfloat*)view);

	glUniformMatrix4fv(model->uniform_locations.proj_loc, 1, GL_FALSE,
		(const GLfloat*)proj);

	

	glDrawArrays(GL_TRIANGLES, 0, model->mesh->vertex_count);
}

static void RenderModel(Model* model, mat4 transform, mat4 view, mat4 proj)
{
	BindModel(model);

	DrawModel(model, transform, view, proj);

	UnbindModel();
}


RenderingSystem RenderingSystemCreate(Game* game, ECS* ecs)
{	
	RenderingSystem system = { 0 };
	
	Signature_t signature = 0;
	signature |= 1 << ENUM_COMPONENT_Transform;
	signature |= 1 << ENUM_COMPONENT_Model;

	system.system = MLECSNewSystem(ecs, signature);
	system.camera = MAX_ENTITIES;

	return system;
}

void RenderingSystemUpdate(Game* game, RenderingSystem* rendering_system, double dt, double alpha)
{
	if (rendering_system->camera == MAX_ENTITIES)
	{
		// try to find the camera
		Entity_t camera_id = NameSystemFind(game, "game_camera");

		if (camera_id != MAX_ENTITIES)
		{
			rendering_system->camera = camera_id;
		}

		return;
	}

	const size_t sz = DynArraySize(&rendering_system->system->entities);

	Entity_t* entities = (Entity_t*)rendering_system->system->entities.data;
	ECS* ecs = GameECS(game);

	//

	mat4 proj;
	mat4 view;
	

	// ah
	const size_t width = game->window_width;
	const size_t height = game->window_height;

	float aspect = (float)width / (float)height;

	glm_perspective(glm_rad(60.0f), aspect, 0.1f, 100.0f, proj);

	Entity_t camera = rendering_system->camera;

	mat4 camera_transform;
	TransformGetInterpolatedWorldTransform(ecs, camera, alpha, camera_transform);
	
	vec3 camera_world_pos;
	camera_world_pos[0] = camera_transform[3][0];
	camera_world_pos[1] = camera_transform[3][1];
	camera_world_pos[2] = camera_transform[3][2];	
	camera_world_pos[2] = camera_transform[3][2];	

	versor camera_rot;
	/*
	TransformGetWorldVersor(ecs, camera, camera_rot);
	glm_quat_normalize(camera_rot);
	*/

	glm_mat4_quat(camera_transform, camera_rot);
	glm_quat_normalize(camera_rot);

	vec3 up = { 0.0f, 1.0f, 0.0f };
	vec3 forward = { 0.0f, 0.0f, -1.0f };

	vec3 forward_vec;
	glm_quat_rotatev(camera_rot, forward, forward_vec);
	glm_normalize(forward_vec);

	vec3 up_vec;
	glm_quat_rotatev(camera_rot, up, up_vec);
	glm_normalize(up_vec);
	
	vec3 target;
	glm_vec3_add(forward_vec, camera_world_pos, target);

	glm_lookat(camera_world_pos, 
		target,
		up_vec,
		view);

	//


	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];

		Model* model = MLECSGetComponentModel(ecs, e);
		
		mat4 interp_transform;
		TransformGetInterpolatedWorldTransform(ecs, e, alpha, interp_transform);

		RenderModel(model, interp_transform, view, proj);
	}
}