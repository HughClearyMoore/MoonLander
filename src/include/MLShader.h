#pragma once

#include <glad/glad.h>

#include <STI_String.h>
#include <DynArray.h>

typedef struct ShaderProgram
{
	GLuint id;
} ShaderProgram;

typedef struct Shader
{
	GLuint id;
} Shader;

typedef struct ShaderBuilder
{
	DynArray sub_shaders;
} ShaderBuilder;

ShaderBuilder MLShaderBuilderCreate();

// note: this moves the shader into the ShaderBuilder so no need to destroy the shader
void MLShaderBuilderAttach(ShaderBuilder* builder, Shader* shader);

ShaderProgram MLShaderBuilderBuild(ShaderBuilder* builder);
void MLShaderBuilderDestroy(ShaderBuilder* builder);

Shader* MLShaderCreate(STI_StringView file_path, GLenum shader_type);

void MLProgramDestroy(ShaderProgram* program);