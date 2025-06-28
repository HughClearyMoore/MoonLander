#include "MLCore.h"

#include <assert.h>
#include <string.h>

#include "MLColours.h"

#include "MLShader.h"
#include "MLMesh.h"
#include "MLTypes.h"
#include "MLModel.h"

#include "MLOBJLoader.h"

#include <stdio.h>

#define DRAW_DEBUG 0

static void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

Game GameCreate(const size_t width, const size_t height, const char* title)
{
	Game game;
	memset(&game, 0, sizeof(game));

	{
		int res = glfwInit();
		assert(res == GLFW_TRUE && "Failed to init GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	game.window_width = width;
	game.window_height = height;
	game.title = STIStringCreate(title);
	
	game.window = glfwCreateWindow(width, height, game.title.data, NULL, NULL);
	assert(game.window && "Failed to create game window");

	glfwMakeContextCurrent(game.window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		assert(0 && "Failed to init GLAD");
	}


	glViewport(0, 0, width, height);

	glfwSetFramebufferSizeCallback(game.window, &WindowResizeCallback);


	game.mesh_manager = MLMeshManagerCreate();

	return game;
}

void GameDestroy(Game* game)
{

	MLMeshManagerDestroy(&game->mesh_manager);

	glfwDestroyWindow(game->window);
	STIStringDestroy(&game->title);


	glfwTerminate();
}

void GameStart(Game* game)
{
	
	DynArray obj = MLLoadOBJFile(STI_StringViewCreateFromCString("../data/meshes/triangle.obj"));
	DynArray obj2 = MLLoadOBJFile(STI_StringViewCreateFromCString("../data/meshes/cube.obj"));
	DynArray obj3 = MLLoadOBJFile(STI_StringViewCreateFromCString("../data/meshes/teapot.obj"));

	Mesh triangle_mesh = MLMeshCreateFromCArray(obj.data, DynArraySize(&obj), sizeof(Vertex));
	Mesh cube_mesh = MLMeshCreateFromCArray(obj2.data, DynArraySize(&obj2), sizeof(Vertex));
	Mesh teapot_mesh = MLMeshCreateFromCArray(obj3.data, DynArraySize(&obj3), sizeof(Vertex));
	
	DynArrayDestroy(&obj);
	DynArrayDestroy(&obj2);
	DynArrayDestroy(&obj3);


	game->is_running = STI_TRUE;

	glClearColor(
		ML_Colour_TardisBlue.r,
		ML_Colour_TardisBlue.g,
		ML_Colour_TardisBlue.b,
		1.0f);

	glEnable(GL_DEPTH_TEST);

#if DRAW_DEBUG

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0);

#endif



	ShaderBuilder builder = MLShaderBuilderCreate();

	Shader* vert = MLShaderCreate(STI_StringViewCreateFromCString("../data/shaders/basic.vert"), GL_VERTEX_SHADER);
	Shader* frag = MLShaderCreate(STI_StringViewCreateFromCString("../data/shaders/basic.frag"), GL_FRAGMENT_SHADER);
	
	MLShaderBuilderAttach(&builder, vert);
	MLShaderBuilderAttach(&builder, frag);

	ShaderProgram program = MLShaderBuilderBuild(&builder);

	MLShaderBuilderDestroy(&builder);

	float vertices[] =
	{
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	Mesh mesh = MLMeshCreateFromCArray(vertices, sizeof(vertices) / sizeof(Vertex), sizeof(Vertex));

	MLMeshManagerAddMesh(&game->mesh_manager, mesh, "triangle");
	MLMeshManagerAddMesh(&game->mesh_manager, triangle_mesh, "triangle_obj");
	MLMeshManagerAddMesh(&game->mesh_manager, cube_mesh, "cube");
	MLMeshManagerAddMesh(&game->mesh_manager, teapot_mesh, "teapot");

	Mesh* entry = MLMeshManagerGetMesh(&game->mesh_manager, "teapot");

	Model model = MLModelCreate(entry, program);

	while (!glfwWindowShouldClose(game->window) && game->is_running)
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		MLModelDraw(&model);

		glfwSwapBuffers(game->window);
	}	
	
	MLProgramDestroy(&program);
}