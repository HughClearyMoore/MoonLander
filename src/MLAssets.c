#include "MLAssets.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>
#include <DynArray.h>
#include <STI_String.h>
#include <Utility.h>

#include "MLFileIO.h"
#include "MLMesh.h"
#include "MLOBJLoader.h"
#include "MLTypes.h"
#include "MLShader.h"



#define X(item) item,
typedef enum AssetType
{	
#include "defs/MLAssetTypes.defs"
	ASSET_TYPE_COUNT,
	INVALID
} AssetType;
#undef X

typedef struct AssetReference
{
	STI_String file_path;
	STI_String asset_name;
	AssetType type;
	
} AssetReference;


#define X(item) void LoadAsset##item(Game* game_ctx, AssetReference* asset_reference);
#include "defs/MLAssetTypes.defs"
#undef X

STI_String FileGetLine(FILE* file)
{
	char buf[1024] = { 0 };

	char* ptr = fgets(buf, sizeof(buf), file);
	
	STI_String line = { 0 };

	if (ptr)
	{
		line = STIStringCreate(buf);
	}	

	return line;
}

static void AssetReferenceDeleter(void* ptr)
{
	AssetReference* ar = (AssetReference*)ptr;

	STIStringDestroy(&ar->asset_name);
	STIStringDestroy(&ar->file_path);

	*ar = (AssetReference){ 0 };
}

static const AssetType AssetTypeFromName(const char* name)
{
#define X(item) if(strcmp(name, #item) == 0) return item;
#include "defs/MLAssetTypes.defs"
#undef X

	return INVALID;
}

static const char* AssetTypeNameFromType(const AssetType type)
{
	switch (type)
	{
#define X(item) case item: return #item;
#include "defs/MLAssetTypes.defs"
#undef X
	}

	return NULL;
}

static AssetReference ParseAssetListLine(STI_StringView line)
{
	AssetReference ar = { 0 };

	// split based on quoation marks

	// "type_name" "name" "file_path"

	STI_BOOL parsed_line_correctly = STI_TRUE;

	char* current_member = line.data;
	for (size_t i = 0; i < 3; ++i)
	{
		char* opening_mark = strstr(current_member, "\"");

		if (opening_mark == NULL || opening_mark[1] == '\0')
		{
			parsed_line_correctly = STI_FALSE;
			break;
		}

		char* closing_mark = strstr(opening_mark + 1, "\"");

		if (closing_mark == NULL)
		{
			parsed_line_correctly = STI_FALSE;
			break;
		}

		current_member = closing_mark + 1;

		const size_t string_size = (size_t)(closing_mark - opening_mark - 1);
		char* string = calloc(string_size + 1, sizeof(char));
		assert(string);
		memcpy(string, opening_mark + 1, string_size);

		string[string_size] = '\0';

		if (i == 0)
		{
			ar.type = AssetTypeFromName(string);

			if (ar.type == INVALID)
			{
				parsed_line_correctly = STI_FALSE;
				break;
			}

			free(string);
		}
		else if (i == 1)
		{
			ar.asset_name = STIStringMoveCreate(string);
		}
		else
		{
			ar.file_path = STIStringMoveCreate(string);
		}
	}

	if (parsed_line_correctly == STI_FALSE)
	{
		// cleanup resources
		ar.type = INVALID;
		
		if (ar.asset_name.data)
		{
			STIStringDestroy(&ar.asset_name);
		}

		if (ar.file_path.data)
		{
			STIStringDestroy(&ar.file_path);
		}		
	}


	return ar;
}

DynArray GetAssetReferences(const char* file_path)
{
	FILE* file = NULL;

	file = fopen(file_path, "rb");
	if (file == NULL)
	{
		fprintf(stderr, "Failed to open asset list file '%s'\n", file_path);		
		assert(0 && "Failed to open file");
	}

	DynArray asset_references = DynArrayCreate(sizeof(AssetReference), 0, &AssetReferenceDeleter);

	STI_String line = FileGetLine(file);

	while (line.data != NULL)
	{
		AssetReference ar = ParseAssetListLine(STI_StringViewCreateFromSTIString(&line));

		if(ar.type != INVALID)
		{
			DynArrayPush(&asset_references, &ar);			
		}

		STIStringDestroy(&line);
		line = FileGetLine(file);
	}

	fclose(file);	
	
	return asset_references;
}

static void LoadAssetReference(AssetReference* asset_reference, void* ctx)
{
	Game* game_ctx = (Game*)ctx;

	switch (asset_reference->type)
	{
#define X(item) case item: LoadAsset##item(game_ctx, asset_reference); break;
#include "defs/MLAssetTypes.defs"
#undef X
	}
}

void MLLoadAssets(Game* game_ctx)
{
	DynArray asset_references = GetAssetReferences("../data/asset_list.txt");

	DynArrayForEach(&asset_references, &LoadAssetReference, game_ctx);

	DynArrayDestroy(&asset_references);
}

void LoadAssetMESH(Game* game_ctx, AssetReference* asset_reference)
{	
	STI_StringView path = STI_StringViewCreateFromSTIString(&asset_reference->file_path);

	DynArray vertex_data = MLLoadOBJFile(path);

	Mesh mesh = MLMeshCreateFromCArray(vertex_data.data, DynArraySize(&vertex_data), sizeof(Vertex));	

	DynArrayDestroy(&vertex_data);	

	if (mesh.vertex_count)
	{
		MLMeshManagerAddMesh(&game_ctx->mesh_manager, mesh, asset_reference->asset_name.data);
	}
}

void LoadAssetTEXTURE(Game* game_ctx, AssetReference* asset_reference)
{
	
}

void LoadAssetSHADER(Game* game_ctx, AssetReference* asset_reference)
{
	// this gets fiddly

	STI_StringView file_path = STI_StringViewCreateFromSTIString(&asset_reference->file_path);

	FILE* shader_config = fopen(file_path.data, "rb");

	assert(shader_config);

	const size_t file_size = MLGetFileSize(shader_config);

	char* src = calloc(file_size + 1, sizeof(char));

	assert(src);

	fread(src, sizeof(char), file_size, shader_config);

	ShaderBuilder sb = MLShaderBuilderCreate();

	char* token = strtok(src, " ");

	while (token != NULL)
	{
		// we need the directory so get the last / and then duplicate previous to that

		char* slash_location = strstr(file_path.data, "/");
		char* last_slash_location = NULL;

		while (slash_location)
		{
			last_slash_location = slash_location;

			if (slash_location[1] == '\0') // maybe add an assert here or some error handling
			{
				break;
			}

			slash_location = strstr(slash_location + 1, "/");
		}

		const size_t directory_last = (size_t)(last_slash_location - file_path.data + 1);		

		// append the directory

		char* shader_path = calloc(directory_last + strlen(token) + 1, sizeof(char));
		assert(shader_path);

		memcpy(shader_path, file_path.data, directory_last);	

		memcpy(shader_path + directory_last, token, strlen(token));

		// full path 

		STI_StringView shader_full_path = STI_StringViewCreateFromCString(shader_path);

		Shader* shader = NULL;

		const char* ext = strstr(token, ".");

		assert(ext);

		if (strcmp(ext, ".vert") == 0)
		{
			shader = MLShaderCreate(shader_full_path, GL_VERTEX_SHADER);
		}
		else if (strcmp(ext, ".frag") == 0)
		{
			shader = MLShaderCreate(shader_full_path, GL_FRAGMENT_SHADER);
		}
		
		free(shader_path);

		MLShaderBuilderAttach(&sb, shader);

		token = strtok(NULL, " ");
	}

	ShaderProgram program = MLShaderBuilderBuild(&sb);

	MLShaderProgramManagerAddProgram(&game_ctx->program_manager, program, asset_reference->asset_name.data);

	MLShaderBuilderDestroy(&sb);
}