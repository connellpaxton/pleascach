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
	vec4 frustum[6];
	vec2 viewport;
	float tess_factor;
	float tess_edge_size;
};

layout (set = 0, binding = 1) uniform sampler2D tex;     

void main() {
	vec3 light_pos = normalize(vec3(cos(time), sin(time), 0.0))*10.0;
	vec3 L = normalize(light_pos-pos);
	float r = length(light_pos-pos);
	float t = clamp(dot(L, norm), 0.0, 1.0) * 20.0/(r*r);

	FragColor = vec4(1.0);
}