#pragma once

#include <HashMap.h>

#include "MLShader.h"

typedef struct ShaderProgramManager
{
	HashMap programs;
} ShaderProgramManager;

ShaderProgramManager MLShaderProgramManagerCreate();
void MLShaderProgramManagerDestroy(ShaderProgramManager* manager);

// note: takes ownership of program
void MLShaderProgramManagerAddProgram(ShaderProgramManager* manager, ShaderProgram program, const char* name);

ShaderProgram* MLShaderProgramManagerGetProgram(ShaderProgramManager* manager, const char* name);