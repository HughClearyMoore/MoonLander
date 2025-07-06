#pragma once

#include <STI_String.h>

typedef struct Name
{
	STI_String name;
} Name;

Name NameComponentCreate(const char* name);