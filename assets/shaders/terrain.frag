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
	vec3 cam_dir;
	vec4 frustum[6];
	vec2 viewport;
	float tess_factor;
	float tess_edge_size;
};

layout (set = 0, binding = 1) uniform sampler2D heightmap;

void main() {
	/* extract L (light direction) from view matrix */
	vec3 L = -cam_dir;
	float r = length(cam_pos-pos);
	float t = clamp(dot(L, norm), 0.0, 1.0) * 20.0/(r*r);

	FragColor = vec4(t*norm, 1.0);
}