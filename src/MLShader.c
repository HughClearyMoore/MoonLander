#include "MLShader.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t get_file_size(FILE* file)
{
	size_t current = ftell(file);
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, current, SEEK_SET);

	return size;
}

static void MLShaderDestroy(Shader** shader)
{
	glDeleteShader((*shader)->id);
	free(*shader);
}

static void ShaderAttacher(Shader** shader, void* ctx)
{
	GLuint shader_id = (*shader)->id;
	ShaderProgram* program = (ShaderProgram*)ctx;

	glAttachShader(program->id, shader_id);
}

static STI_String MLGetShaderSource(STI_StringView file_path)
{
	STI_String src = { 0 };

	FILE* file = fopen(file_path.data, "rb");

	if (file == NULL)
	{
		fprintf(stderr, "Failed to open file %s\n", file_path.data);
		assert(0 && "File open failed");
	}

	const size_t file_size = get_file_size(file);

	char* src_code = calloc(file_size + 1, sizeof(char));
	assert(src_code && "Failed to allocate memory for shader source code");


	const size_t ret_code = fread(src_code, sizeof(char), file_size, file);

	assert(ret_code == file_size && "Failed to read data from file");

	src_code[file_size] = '\0';

	fclose(file);

	src = STIStringMoveCreate(src_code);

	return src;
}


ShaderBuilder MLShaderBuilderCreate()
{
	ShaderBuilder sb = { 0 };
	sb.sub_shaders = DynArrayCreate(sizeof(Shader*), 0, &MLShaderDestroy);

	return sb;
}

void MLShaderBuilderDestroy(ShaderBuilder* builder)
{
	DynArrayDestroy(&builder->sub_shaders);
	*builder = (ShaderBuilder){ 0 };
}

void MLShaderBuilderAttach(ShaderBuilder* builder, Shader* shader)
{
	assert(shader && "Tried to attach null shader");
	
	DynArrayPush(&builder->sub_shaders, &shader);
}

ShaderProgram MLShaderBuilderBuild(ShaderBuilder* builder)
{
	ShaderProgram program = { 0 };


	program.id = glCreateProgram();

	assert(program.id && "Failed to create shader program");

	DynArrayForEach(&builder->sub_shaders, &ShaderAttacher, &program);

	glLinkProgram(program.id);

	GLint program_linked;

	glGetProgramiv(program.id, GL_LINK_STATUS, &program_linked);

	if (program_linked == GL_FALSE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];

		glGetProgramInfoLog(program.id, sizeof(message), &log_length, message);
		fprintf(stderr, "Failed to link shader program: %s\n", message);
		assert(0 && "Failed to link shader program");
	}

	return program;
}


Shader* MLShaderCreate(STI_StringView file_path, GLenum shader_type)
{
	Shader* shader = calloc(1, sizeof(Shader));
	assert(shader && "Failed to allocate memory for shader");


	shader->id = glCreateShader(shader_type);

	if (shader->id == 0)
	{
		fprintf(stderr, "Failed to create shader with file path '%s' with shader_type '%d'\n", file_path.data, shader_type);
		assert(0 && "Failed to create shader");
	}	

	STI_String src = MLGetShaderSource(file_path);
	
	// compile shader

	glShaderSource(shader->id, 1, &src.data, NULL);

	glCompileShader(shader->id);

	GLint vertex_compiled;

	glGetShaderiv(shader->id, GL_COMPILE_STATUS, &vertex_compiled);

	if (vertex_compiled == GL_FALSE)
	{
		GLsizei log_length = 0;
		GLchar message[1024];

		glGetShaderInfoLog(shader->id, sizeof(message), &log_length, message);
		fprintf(stderr, "Failed to compile shader at '%s':\n%s\n", file_path.data, message);
		assert(0 && "Failed to compile shader");
	}


	STIStringDestroy(&src);

	return shader;
}

void MLProgramDestroy(ShaderProgram* program)
{
	assert(program && "Tried to delete null program");
	glDeleteProgram(program->id);
	*program = (ShaderProgram){ 0 };
}