#include "MLInput.h"

#include <assert.h>
#include <stdio.h>

#include "MLCore.h"


static void PushMarkedKey(Input* input, int key)
{
	size_t* marked_count = &input->marked.marked_count;

	assert(*marked_count < ML_MARK_COUNT);

	input->marked.marked[*marked_count] = key;
	(*marked_count)++;
}

static void MarkKey(Input* input, int key)
{
	input->marked.is_marked[key] = STI_TRUE;
	PushMarkedKey(input, key);
}

inline static void HandleKeyPress(Input* input, int key, int scancode, int mods)
{
	if (key >= 0 && key < ML_KEY_COUNT)
	{		
		if (!input->keys[key].is_pressed)
		{
			input->keys[key].just_pressed = STI_TRUE;
			input->any_key.just_pressed = STI_TRUE;
			MarkKey(input, key);
		}		

		input->any_key.is_pressed = STI_TRUE;
		input->keys[key].is_pressed = STI_TRUE;
	}
}

inline static void HandleKeyRelease(Input* input, int key, int scancode, int mods)
{
	if (key >= 0 && key < ML_KEY_COUNT)
	{		
		input->keys[key].just_pressed = STI_FALSE;
		input->keys[key].is_pressed = STI_FALSE;
		input->keys[key].just_released = STI_TRUE;

		input->any_key.just_pressed = STI_FALSE;
		input->any_key.is_pressed = STI_FALSE;
		input->any_key.just_released = STI_TRUE;

		MarkKey(input, key);
	}
}

void MLInputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Game* game = glfwGetWindowUserPointer(window);
	assert(game);
	Input* input = &game->input;
	assert(input);

	switch (action)
	{
	case GLFW_PRESS:
		HandleKeyPress(input, key, scancode, mods);
		break;
	case GLFW_RELEASE:
		HandleKeyRelease(input, key, scancode, mods);
		break;
	case GLFW_REPEAT:
		// maybe you want this for some reason?
		break;
	default:
		break;
	}
}

void MLInputResetMarked(Input* input)
{
	const size_t sz = input->marked.marked_count;

	for (size_t i = 0; i < sz; ++i)
	{
		int key_idx = input->marked.marked[i];

		Key* key = &input->keys[key_idx];
		key->just_pressed = STI_FALSE;
		key->just_released = STI_FALSE;
	}

	input->any_key.just_pressed = STI_FALSE;
	input->any_key.just_released = STI_FALSE;

	input->marked.marked_count = 0;
}