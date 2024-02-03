#version 460 core

layout (location = 0) in vec3 norm;
layout (location = 1) in vec2 texCoord;
layout (location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mvp;
	float time;
};

layout (set = 0, binding = 1) uniform sampler2D tex;     

void main() {
	FragColor = vec4(norm, 1.0);
}	