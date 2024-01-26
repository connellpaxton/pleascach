#version 460 core
layout (location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mpv;
	float time;
};

void main() {
//  FragColor = vec4(abs(cos(time)), 0.5, 1.0, 1.0);
	FragColor = vec4(1.0);
}