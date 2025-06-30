#include "MLShaderProgramManager.h"

#include <assert.h>
#include <stdlib.h>

static void ProgramDeleter(void** ptr)
{
	ShaderProgram* program = *(ShaderProgram**)ptr;

	MLProgramDestroy(program);
	free(program);
}

ShaderProgramManager MLShaderProgramManagerCreate()
{
	ShaderProgramManager manager = { 0 };

	HashMap map = HashMapCreate(
		HASHMAP_KEY_STRING,
		sizeof(ShaderProgram*),
		128,
		&HashMapFNV1AHash,
		&HashMapStringCmp,
		&ProgramDeleter
	);

	manager.programs = map;

	return manager;
}
void MLShaderProgramManagerDestroy(ShaderProgramManager* manager)
{
	HashMapDestroy(&manager->programs);
	*manager = (ShaderProgramManager){ 0 };
}

// note: takes ownership of program
void MLShaderProgramManagerAddProgram(ShaderProgramManager* manager, ShaderProgram program, const char* name)
{
	ShaderProgram* program_ptr = (ShaderProgram*)calloc(1, sizeof(ShaderProgram));
	assert(program_ptr);

	*program_ptr = program;
	

	HashMapInsert(&manager->programs, name, &program_ptr);
}

ShaderProgram* MLShaderProgramManagerGetProgram(ShaderProgramManager* manager, const char* name)
{
	return *(ShaderProgram**)HashMapGet(&manager->programs, name);
}