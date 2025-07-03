#pragma once

#include <Utility.h>

#define ML_KEY_COUNT 500
#define ML_MARK_COUNT 32

typedef struct GLFWwindow GLFWwindow;

typedef struct Key
{
	STI_BOOL is_pressed;
	STI_BOOL just_pressed;
	STI_BOOL just_released;
} Key;

typedef struct Input
{
	Key keys[ML_KEY_COUNT];
	Key any_key;
	struct Marked
	{
		size_t marked[ML_MARK_COUNT];
		STI_BOOL is_marked[ML_KEY_COUNT];
		size_t marked_count;
	} marked;	
} Input;

void MLInputKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void MLInputResetMarked(Input* input);