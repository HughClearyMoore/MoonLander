#pragma once

#include <Utility.h>

#define ML_KEY_COUNT 500

typedef struct GLFWwindow GLFWwindow;

typedef struct Key
{
	STI_BOOL is_pressed;
} Key;

typedef struct Input
{
	Key keys[ML_KEY_COUNT];
} Input;

void MLInputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);