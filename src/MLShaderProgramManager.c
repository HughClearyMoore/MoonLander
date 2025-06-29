#include "MLShaderProgramManager.h"

ShaderProgramManager MLShaderProgramManagerCreate()
{
	ShaderProgramManager manager = { 0 };

	HashMap map = HashMapCreate(
		HASHMAP_KEY_STRING,
		sizeof(ShaderProgram),
		128,
		&HashMapFNV1AHash,
		&HashMapStringCmp,
		MLProgramDestroy
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
	HashMapInsert(manager, name, &program);
}

ShaderProgram* MLShaderProgramManagerGetProgram(ShaderProgramManager* manager, const char* name)
{
	return (ShaderProgram*)HashMapGet(manager, name);
}