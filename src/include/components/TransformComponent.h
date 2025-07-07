#pragma once

typedef struct Transform
{
	double x, y, z;
	double rx, ry, rz;
	double scale;

	double prev_x, prev_y, prev_z;
} Transform;