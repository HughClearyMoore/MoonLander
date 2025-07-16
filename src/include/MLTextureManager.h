#pragma once

#include <glad/glad.h>

#include <HashMap.h>

typedef struct MLTexture
{
	GLuint texture_id;
	size_t width;
	size_t height;
} MLTexture;

typedef struct MLTextureManager
{
	HashMap textures; // HashMap<MLTexture*>
} MLTextureManager;

MLTextureManager MLTextureManagerCreate();
void MLTextureManagerDestroy(MLTextureManager* manager);

MLTexture* MLTextureManagerGet(MLTextureManager* manager, const char* name);
void MLTextureManagerInsert(MLTextureManager* manager, const char* name, const void* data, size_t width, size_t height);