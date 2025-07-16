#include "MLTextureManager.h"

#include <assert.h>
#include <stdlib.h>

static void TextureDeleter(void* ptr)
{
	MLTexture* texture = *(MLTexture**)ptr;

	if (texture)
	{
		glDeleteTextures(1, &texture->texture_id);
		free(texture);
	}	
}

MLTextureManager MLTextureManagerCreate()
{
	MLTextureManager manager = { 0 };

	manager.textures = HashMapCreate(
		HASHMAP_KEY_STRING,
		sizeof(MLTexture*),
		128,
		&HashMapFNV1AHash,
		&HashMapStringCmp,
		&TextureDeleter
	);

	return manager;
}

void MLTextureManagerDestroy(MLTextureManager* manager)
{
	HashMapDestroy(&manager->textures);
}

MLTexture* MLTextureManagerGet(MLTextureManager* manager, const char* name)
{
	return *(MLTexture**)HashMapGet(manager, name);
}

void MLTextureManagerInsert(MLTextureManager* manager, const char* name, const void* data, size_t width, size_t height)
{
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		(const GLvoid*)data
	);

	glBindTexture(GL_TEXTURE_2D, 0);

	MLTexture* ml_texture = malloc(sizeof(MLTexture));
	
	assert(ml_texture);

	ml_texture->texture_id = texture;
	ml_texture->width = width;
	ml_texture->height = height;

	HashMapInsert(&manager->textures, name, &ml_texture);
}