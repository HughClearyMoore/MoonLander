#include "MLFileIO.h"

size_t MLGetFileSize(FILE* file)
{
	size_t current = ftell(file);
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, current, SEEK_SET);

	return size;
}