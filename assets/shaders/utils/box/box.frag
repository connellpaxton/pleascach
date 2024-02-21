#version 460 core
layout (location = 0) flat in uint id;

layout (location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 view;
	mat4 proj;
	float time;
};

void main() {
	FragColor = vec4(0.0, 0.0, 1.0, 0.2);
}