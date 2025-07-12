#include "scripts/teapot_script.h"

#include "MLScript.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <cglm/cglm.h>

#include "MLCore.h"

static double MakeRandom()
{
	return rand() / (double)RAND_MAX;
}

DEFAULT_CREATE_FUNCTION(TeapotScript, TeapotCtx)

READY_FUNCTION(TeapotScript)
{
	TeapotCtx* teapot = (TeapotCtx*)ctx;
	printf("I, the Teapot, have been born!\n");
	
	teapot->life_force = 100.0;
}

UPDATE_FUNCTION(TeapotScript)
{
	Input* input = GET_INPUT;
	TeapotCtx* teapot = (TeapotCtx*)ctx;

	Transform* t = MLECSGetComponentTransform(&game_ctx->ecs, entity);

	double scale = teapot->life_force / 100.0;
	scale = scale < 0.0 ? 0.0 : scale;

	TransformUniformScale(t, scale);
	
	if (scale <= 0.0)
	{
		MLECSDestroyEntity(&game_ctx->ecs, entity);
	}


	vec3 delta_rot_axis = { 1.0, 0.0, 1.0 };
	glm_normalize(delta_rot_axis);
	TransformRotate(t, delta_rot_axis, dt);


	if (input->keys[GLFW_KEY_LEFT_SHIFT].just_pressed)
	{
		Entity_t new_entity = MLECSNewEntity(&game_ctx->ecs);

		
		vec3 direction = { cos(MakeRandom() * 2 * 3.141592653), sin(MakeRandom() * 2 * 3.141592653), sin(MakeRandom() * 2 * 3.141592653)};
		glm_normalize(direction);

		double distance = 1 + MakeRandom() * 10.0;

		glm_vec3_scale(direction, (float)distance, direction);

		vec3 final_pos = { 0.0, 0.0, 0 };
		glm_vec3_add(final_pos, direction, final_pos);

		Transform trans = TransformIdentity();

		trans.position.x = final_pos[0];
		//trans.position.y = final_pos[1];
		trans.position.z = final_pos[2];
		
		trans.position.y = 0.0;

		TransformUniformScale(&trans, MakeRandom() * 3 + 1.0);		

		MLECSAttachComponentTransform(&game_ctx->ecs, new_entity, &trans);

		Mesh* mesh = MLMeshManagerGetMesh(&game_ctx->mesh_manager, "cube");
		ShaderProgram* program = MLShaderProgramManagerGetProgram(&game_ctx->program_manager, "basic");

		Model model = CreateModel(mesh, program);

		MLECSAttachComponentModel(&game_ctx->ecs, new_entity, &model);

		dMass mass;
		dMassSetBox(&mass, 1.0, 1.0, 1.0, 1.0);

		RigidBody body = RigidBodyCreate(&game_ctx->ecs, new_entity, PhysicsSystemCurrentWorld(game_ctx), &mass);

		MLECSAttachComponentRigidBody(GameECS(game_ctx), new_entity, &body);


		// let's attempt the impossible
		double box_scale = trans.scale.scale_x;
		dGeomID box = dCreateBox(0, box_scale, box_scale, box_scale);
		Collider collider = ColliderDynamicCreate(game_ctx, new_entity, box, 0.5, 0.01, 0.01);
		MLECSAttachComponentCollider(GameECS(game_ctx), new_entity, &collider);

		// temporary
		Script scr = ScriptComponentCreate(game_ctx, SCRIPT_ENUM_ElephantScript);
		MLECSAttachComponentScript(GameECS(game_ctx), new_entity, &scr);
	}
}

DESTROY_FUNCTION(TeapotScript)
{
	printf("I, the Teapot, have been eviscerated!\n");
}