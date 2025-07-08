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

static void DrawModel(Model* model, mat4 transform, mat4 viewproj)
{
	GLuint modelLoc = glGetUniformLocation(model->program->id, "uModel");
	GLuint viewLoc = glGetUniformLocation(model->program->id, "uViewProj");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (const GLfloat*)transform);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (const GLfloat*)viewproj);

	glDrawArrays(GL_TRIANGLES, 0, model->mesh->vertex_count);
}

static void RenderModel(Model* model, mat4 transform, mat4 viewProj)
{
	BindModel(model);

	DrawModel(model, transform, viewProj);


	UnbindModel();
}

static void RenderPhysicsModel(Model* model, Transform* transform, mat4 viewProj, double alpha)
{
	BindModel(model);

	DrawPhysicsModel(model, transform, viewProj, alpha);

	UnbindModel();
}


RenderingSystem RenderingSystemCreate(Game* game)
{
	ECS* ecs = &game->ecs;
	RenderingSystem system = { 0 };
	
	Signature_t signature = 0;
	signature |= 1 << ENUM_COMPONENT_Transform;
	signature |= 1 << ENUM_COMPONENT_Model;

	system.system = MLECSNewSystem(ecs, signature);

	return system;
}

void RenderingSystemInitialise(RenderingSystem* rendering_system, Game* game)
{
	ECS* ecs = &game->ecs;	
	// construct a camera

	rendering_system->camera = MLECSNewEntity(ecs);

	Transform transform = TransformIdentity();	

	MLECSAttachComponentTransform(ecs, rendering_system->camera, &transform);

	Script script = ScriptComponentCreate(game, SCRIPT_ENUM_PlayerScript);

	MLECSAttachComponentScript(ecs, rendering_system->camera, &script);

	//
}

void RenderingSystemUpdate(Game* game, RenderingSystem* rendering_system, double dt, double alpha)
{

	const size_t sz = DynArraySize(&rendering_system->system->entities);

	Entity_t* entities = (Entity_t*)rendering_system->system->entities.data;
	ECS* ecs = &game->ecs;

	//

	mat4 proj;
	mat4 view;
	mat4 viewproj;

	// ah
	const size_t width = game->window_width;
	const size_t height = game->window_height;

	float aspect = (float)width / (float)height;

	glm_perspective(glm_rad(60.0f), aspect, 0.1f, 100.0f, proj);

	Transform* camera_transform = MLECSGetComponentTransform(ecs, rendering_system->camera);

	versor camera_rot;
	TransformGetVersor(camera_transform, camera_rot);	

	glm_quat_normalize(camera_rot);

	vec3 up = { 0.0f, 1.0f, 0.0f };
	vec3 forward = { 0.0f, 0.0f, -1.0f };

	vec3 forward_vec;
	glm_quat_rotatev(camera_rot, forward, forward_vec);
	glm_normalize(forward_vec);

	vec3 up_vec;
	glm_quat_rotatev(camera_rot, up, up_vec);
	glm_normalize(up_vec);

	vec3 camera_pos = { (float)camera_transform->position.x, (float)camera_transform->position.y, (float)camera_transform->position.z };
	
	vec3 target;
	glm_vec3_add(forward_vec, camera_pos, target);

	glm_lookat(camera_pos, 
		target,
		up_vec,
		view);

	glm_mat4_mul(proj, view, viewproj);

	//


	for (size_t i = 0; i < sz; ++i)
	{
		Entity_t e = entities[i];

		Model* model = MLECSGetComponentModel(ecs, e);
		
		mat4 interp_transform;
		TransformGetInterpolatedWorldTransform(ecs, e, alpha, interp_transform);

		RenderModel(model, interp_transform, viewproj);
	}
}