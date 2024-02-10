#pragma once

#include <util/glsl_types.hpp>

using namespace glsl;


enum Shape {
	SPHERE,
	BOX,
};

struct Object {
	vec4 center;
	vec4 dimensions;
	uint id;
	Shape shape;
	float pad0;
	float pad1;
};
