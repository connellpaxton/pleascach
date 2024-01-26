#version 460 core

layout (location = 0) in vec2 texCoord;
layout (location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mpv;
	float time;
};

void main() {
	FragColor = vec4(cos(time), 1.0, 0.0, 1.0);
}