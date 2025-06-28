#include "MLCore.h"

#include <assert.h>
#include <string.h>

#include "MLColours.h"

#include "MLShader.h"
#include "MLMesh.h"
#include "MLTypes.h"
#include "MLModel.h"

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

	return game;
}

void GameDestroy(Game* game)
{

	glfwDestroyWindow(game->window);
	STIStringDestroy(&game->title);


	glfwTerminate();
}

void GameStart(Game* game)
{
	game->is_running = STI_TRUE;

	glClearColor(
		ML_Colour_TardisBlue.r,
		ML_Colour_TardisBlue.g,
		ML_Colour_TardisBlue.b,
		1.0f);	

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

	Mesh mesh = MLMeshCreateFromCArray(vertices, sizeof(vertices) / sizeof(Vertex));

	GLuint VAO;
	glGenVertexArrays(1, &VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size * sizeof(Vertex), mesh.vertices.data, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);	

	Model model = MLModelCreate(&mesh, VBO, program);

	while (!glfwWindowShouldClose(game->window) && game->is_running)
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		MLModelDraw(&model);

		glfwSwapBuffers(game->window);
	}

	glBindBuffer(GL_STATIC_DRAW, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	MLMeshDestroy(&mesh);
	MLProgramDestroy(&program);
}