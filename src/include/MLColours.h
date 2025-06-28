#pragma once

typedef struct MLColour
{
	float r, g, b;
} MLColour;

#define X(name, r, g, b) \
	static const MLColour ML_Colour_##name = {r, g, b};

#include "../defs/MLColours.defs"

#undef X
