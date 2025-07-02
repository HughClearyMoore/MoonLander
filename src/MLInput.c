#include "MLInput.h"

#include <assert.h>
#include <stdio.h>

#include "MLCore.h"

void MLInputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Game* game = glfwGetWindowUserPointer(window);
	assert(game);

	switch (action)
	{
	case GLFW_PRESS:
		printf("Key pressed: {'%d'}\n", key);
		break;
	default:
		break;
	}
}