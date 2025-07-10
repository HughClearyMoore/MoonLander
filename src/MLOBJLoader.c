#include "MLOBJLoader.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "MLFileIO.h"
#include "MLTypes.h"

typedef struct OBJContext
{
	DynArray vertex_array;
	DynArray vertex_normal_array;
	DynArray vertex_texture_array;

	DynArray full_vertex_array;
} OBJContext;

static size_t strtoz(const char* str, char** endptr)
{
#if UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
	return (size_t)strtoull(str, endptr, 10);
#elif UINTPTR_MAX == 0xFFFFFFFF	
	return (size_t)strtoul(str, endptr, 10);
#endif	
}

static size_t strcount(const char* str, const char c)
{
	size_t count = 0;
	size_t sz = strlen(str);

	for (size_t i = 0; i < sz; ++i)
	{
		if (c == str[i]) ++count;
	}

	return count;
}

static void StringDeleter(void* string)
{
	char** str = (char**)string;
	free(*str);
}

static DynArray ParseVertex(char* token_in)
{
	DynArray results = DynArrayCreate(sizeof(float), 0, NULL);
	char* token = token_in;

	while (token != NULL)
	{
		float component = strtof(token, NULL);

		DynArrayPush(&results, &component);

		token = strtok(NULL, " ");
	}

	return results;
}

static DynArray ParseFace(char* token_in, DynArray* vertices, DynArray* vertex_normals, DynArray* vertex_textures)
{
	

	DynArray sub_strings = DynArrayCreate(sizeof(char*), 0, &StringDeleter);

	char* token = token_in;

	while (token != NULL)
	{
		char* str = strdup(token);
		DynArrayPush(&sub_strings, &str);
		
		token = strtok(NULL, " ");
	}

	const size_t face_vertex_count = DynArraySize(&sub_strings);

	DynArray result = DynArrayCreate(sizeof(Vertex), 0, NULL);

	// should be like v/vt/vn
	// but any can be missing but v
	// so v// is invalid same as v/

	size_t size = DynArraySize(&sub_strings);	

	Vertex packed_face[3] = { 0 };	

	const size_t type_count = strcount(*(char**)DynArrayGet(&sub_strings, 0), '/') + 1;

	size_t packed_vertex[3] = { 0 };

	for (size_t i = 0; i < 3; ++i)
	{
		char* sub_str = *(char**)DynArrayGet(&sub_strings, i);		

		char buf[16] = { 0 };
		
		size_t buf_len = 0;
		size_t index = 0;
		const size_t sz = strlen(sub_str);

		for (size_t i = 0; i < sz; ++i)
		{
			char c = sub_str[i];

			if (c == '/' || c == '\0')
			{
				if (buf_len > 0)
				{
					buf[buf_len] = '\0';
					packed_vertex[index] = strtoz(buf, NULL);
					buf_len = 0;
					memset(buf, 0, sizeof(buf));
				}
				++index;
			}
			else if(buf_len < sizeof(buf) - 1)
			{
				buf[buf_len++] = c;
			}
		}

		if (buf_len > 0)
		{
			packed_vertex[index] = strtoz(buf, NULL);
		}

		Vertex v = { 0 };

		if (packed_vertex[0])
		{
			size_t idx = (packed_vertex[0] - 1) * 3;
			v.x = *(float*)DynArrayGet(vertices, idx + 0);
			v.y = *(float*)DynArrayGet(vertices, idx + 1);
			v.z = *(float*)DynArrayGet(vertices, idx + 2);
		}

		if (packed_vertex[1])
		{
			size_t idx = (packed_vertex[1] - 1) * 2;
			v.u = *(float*)DynArrayGet(vertex_textures, idx + 0);
			v.v = *(float*)DynArrayGet(vertex_textures, idx + 1);
		}

		if (packed_vertex[2])
		{
			size_t idx = (packed_vertex[2] - 1) * 3;
			v.xn = *(float*)DynArrayGet(vertex_normals, idx + 0);
			v.yn = *(float*)DynArrayGet(vertex_normals, idx + 1);
			v.zn = *(float*)DynArrayGet(vertex_normals, idx + 2);			
		}

		packed_face[i] = v;
		memset(packed_vertex, 0, sizeof(packed_vertex));
	}

	DynArrayReserve(&result, DynArraySize(&result) + 3);
	DynArrayBatchPush(&result, packed_face, 3, 0);

	DynArrayDestroy(&sub_strings);

	return result;
}

static void ParseLine(void* string_ptr, void* ctx)
{
	enum
	{
		VERTEX = 1,
		VERTEX_NORMAL,
		VERTEX_TEXTURE,
		FACE
	};


	char* string = *(char**)string_ptr;
	OBJContext* obj_ctx = (OBJContext*)ctx;

	char* token;

	token = strtok(string, " ");
	size_t type = 0;
	size_t vector_component_size = 0;
	size_t vector_texture_component_size = 0;
	size_t vector_normal_component_size = 0;

	DynArray* Vertices = &obj_ctx->vertex_array;
	DynArray* Vertex_Normals = &obj_ctx->vertex_normal_array;
	DynArray* Vertex_Textures = &obj_ctx->vertex_texture_array;

	DynArray* full_vertices = &obj_ctx->full_vertex_array;

	for (size_t count = 0; token != NULL; ++count)
	{
		// for now assume in order
		// and assume we only care about v
		if (count == 0)
		{
			/*
			if (strlen(token) != 1)
			{
				continue;
			}
			*/
			if (strcmp(token, "v") == 0)
			{
				type = VERTEX;
			}
			else if (strcmp(token, "f") == 0)
			{
				type = FACE;
			}
			else if (strcmp(token, "vt") == 0)
			{
				type = VERTEX_TEXTURE;
			}
			else if (strcmp(token, "vn") == 0)
			{
				type = VERTEX_NORMAL;
			}
		}
		else
		{
			switch (type)
			{
				case VERTEX:
				{
					DynArray vertex = ParseVertex(token);

					if (vector_component_size == 0) vector_component_size = DynArraySize(&vertex);

					DynArrayReserve(Vertices, DynArraySize(Vertices) + DynArraySize(&vertex));
					DynArrayBatchPush(Vertices, vertex.data, DynArraySize(&vertex), DynArraySize(Vertices));
					DynArrayDestroy(&vertex);
					
					break;
				}
				case FACE:
				{
					DynArray full_face = ParseFace(token, Vertices, Vertex_Normals, Vertex_Textures);
					

					DynArrayReserve(full_vertices, DynArraySize(full_vertices) + DynArraySize(&full_face));

					DynArrayBatchPush(full_vertices, full_face.data, DynArraySize(&full_face), DynArraySize(full_vertices));


					DynArrayDestroy(&full_face);


					break;
				}
				case VERTEX_TEXTURE:
				{
					DynArray vertex = ParseVertex(token);

					if (vector_texture_component_size == 0) vector_texture_component_size = DynArraySize(&vertex);

					DynArrayReserve(Vertex_Textures, DynArraySize(Vertex_Textures) + DynArraySize(&vertex));
					DynArrayBatchPush(Vertex_Textures, vertex.data, DynArraySize(&vertex), DynArraySize(Vertex_Textures));
					DynArrayDestroy(&vertex);

					break;
				}
				case VERTEX_NORMAL:
				{
					DynArray vertex = ParseVertex(token);

					if (vector_normal_component_size == 0) vector_normal_component_size = DynArraySize(&vertex);

					DynArrayReserve(Vertex_Normals, DynArraySize(Vertex_Normals) + DynArraySize(&vertex));
					DynArrayBatchPush(Vertex_Normals, vertex.data, DynArraySize(&vertex), DynArraySize(Vertex_Normals));
					DynArrayDestroy(&vertex);

					break;
				}
			}

			break;
		}

		token = strtok(NULL, " ");
	}
}

static DynArray ParseObj(DynArray* array)
{
	OBJContext ctx;

	ctx.vertex_array = DynArrayCreate(sizeof(float), 0, NULL);
	ctx.vertex_normal_array = DynArrayCreate(sizeof(float), 0, NULL);
	ctx.vertex_texture_array = DynArrayCreate(sizeof(float), 0, NULL);

	ctx.full_vertex_array = DynArrayCreate(sizeof(Vertex), 0, NULL);

	DynArrayForEach(array, &ParseLine, &ctx);

	DynArrayDestroy(&ctx.vertex_array);
	DynArrayDestroy(&ctx.vertex_normal_array);
	DynArrayDestroy(&ctx.vertex_texture_array);

	return ctx.full_vertex_array;
}


DynArray MLLoadOBJFile(STI_StringView file_path)
{
	FILE* file = NULL;

	file = fopen(file_path.data, "rb");

	if (file == NULL)
	{
		fprintf(stderr, "Failed to open file '%s'\n", file_path.data);
		assert(0 && "Failed to open file");
	}

	const size_t file_size = MLGetFileSize(file);

	char* buffer = calloc(file_size + 1, sizeof(char));
	assert(buffer);
	size_t ret = fread(buffer, sizeof(char), file_size, file);

	assert(ret == file_size && "Failed to read file");

	fclose(file);

	DynArray arr = DynArrayCreate(sizeof(char*), 0, &StringDeleter);

	// break up into lines

	// let's make an assumption for now

	char line[256] = { 0 };
	size_t line_size = 0;

	for (size_t i = 0; i < file_size; ++i)
	{
		char c = buffer[i];

		if (c == '\r')
		{
			// next character is probably a '\n'
			// skip that shizz
			if (i + 1 < file_size)
			{
				char next = buffer[i + 1];
				if (next == '\n') ++i;				
			}
		}

		if (c == '\n' || c == '\r')
		{
			line[line_size] = '\0';
			char* str = strdup(line);
			DynArrayPush(&arr, &str);
			memset(line, 0, sizeof(line));
			line_size = 0;
			continue;
		}

		if (line_size < sizeof(line) - 1) line[line_size++] = c;

	}

	if (line_size > 0)
	{		
		line[line_size] = '\0';
		char* str = strdup(line);
		DynArrayPush(&arr, &str);
	}

	DynArray vertex_array = ParseObj(&arr);

	DynArrayDestroy(&arr);

	return vertex_array;
}