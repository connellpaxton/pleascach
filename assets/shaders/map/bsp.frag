#version 460 core

layout (location = 0) in vec4 color;
layout (location = 1) in vec2 texCoord;

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

vec3 tex_map(vec2 coords) {
	return vec3(
		1-abs(cos(normalize(texCoord.xxy)))
//		0.0,
//		sin(texCoord.y)
//		sin(texCoord.x/texCoord.y)
	);
}

void main() {
	FragColor.xyz = tex_map(texCoord);
	FragColor.w = 1.0;
}	