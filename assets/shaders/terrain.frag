#version 460 core

layout (location = 0) in vec3 norm;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 pos;
layout (location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 view;
	mat4 proj;
	float time;
	vec3 cam_pos;
	vec2 viewport;
	float tess_factor;
	float tess_edge_size;
};

layout (set = 0, binding = 1) uniform sampler2D tex;     

void main() {
	FragColor = vec4(1.0);
}