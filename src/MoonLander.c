#include <stdio.h>
#include <assert.h>

#include <GLFW/glfw3.h>

int main(int argc, char** argv)
{
	int glfw_result = glfwInit();

	assert(glfw_result == GLFW_TRUE && "Failed to initialise GLFW");

	GLFWwindow* window = glfwCreateWindow(600, 480, "Moon Lander", NULL, NULL);

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();




	}




	glfwDestroyWindow(window);

	glfwTerminate();
	return 0x0;
}