#version 460 core

layout (location = 0) in vec2 texCoord;
layout (location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mpv;
	float time;
};

layout (set = 0, binding = 1) uniform sampler2D tex;     

void main() {
	FragColor = texture(tex, texCoord);
}