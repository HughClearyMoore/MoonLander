#pragma once

#include <stddef.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <STI_String.h>
#include <Utility.h>

#include "MLMeshManager.h"
#include "MLShaderProgramManager.h"
#include "MLScriptManager.h"
#include "MLSceneManager.h"
#include "MLTextureManager.h"
#include "MLInput.h"
#include "ECS/ECS.h"

typedef struct Game
{
	GLFWwindow* window;
	size_t window_width;
	size_t window_height;
	STI_String title;
	STI_BOOL is_running;
	MeshManager mesh_manager;
	ShaderProgramManager program_manager;
	ScriptManager script_manager;
	Input input;

	struct
	{
		SceneManager scene_manager;
		MLTextureManager texture_manager;
	} managers;

} Game;

Game GameCreate(const size_t width, const size_t height, const char* title);
void GameDestroy(Game* game);

void GameStart(Game* game);

ECS* GameECS(Game* game);

Input* GameInput(Game* game);

Entity_t GameCreateCamera(Game* game, Transform* transform);