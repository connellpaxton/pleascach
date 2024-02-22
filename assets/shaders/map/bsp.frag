#version 460 core

layout (location = 0) in vec4 color;
layout (location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 view;
	mat4 proj;
	float time;
	vec3 cam_pos;
	vec3 cam_dir;
	vec4 frustum[6];
	vec2 viewport;
	float tess_factor;
	float tess_edge_size;
};

layout (set = 0, binding = 1) uniform sampler2D tex;     

void main() {
	FragColor = (color + vec4(1.0)) / 2.0;
}	