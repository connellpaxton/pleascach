#version 460 core

layout (location = 0) in vec2 aCoord;

layout (location = 0) out vec3 pos;
layout (location = 1) out vec3 dir;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 view;
	vec2 viewport;
	float time;
	float focal_length;
};

void main() {
	vec4 p = view * vec4(aCoord.x * viewport.x/viewport.y, aCoord.y, 0.0, 1.0);
	pos = p.xyz;
	p-= view * vec4(0.0, 0.0, -focal_length, 1.0);
	dir = normalize(p.xyz);

	gl_Position = vec4(aCoord, 0.0, 1.0);
}