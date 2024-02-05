#version 450 core

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mvp;
	float time;
	vec3 cam_pos;
	vec2 viewport;
	float tess_factor;
	float tess_edge_size;
};

layout(set = 0, binding = 1) uniform sampler2D heightmap;

layout(quads, equal_spacing, ccw) in;


layout (location = 0) in vec3 norm[];
layout (location = 1) in vec2 texCoord[];
layout (location = 2) in vec3 pos[];

layout (location = 0) out vec3 _norm;
layout (location = 1) out vec2 _texCoord;
layout (location = 2) out vec3 _pos;

void main() {
	/* interpolation of UV coordinates */
	vec2 uv1 = mix(texCoord[0], texCoord[1], gl_TessCoord.x);
	vec2 uv2 = mix(texCoord[3], texCoord[2], gl_TessCoord.x);
	_texCoord = mix(uv1, uv2, gl_TessCoord.y);
	

	vec3 norm1 = mix(norm[0], norm[1], gl_TessCoord.x);
	vec3 norm2 = mix(norm[3], norm[2], gl_TessCoord.x);
	_norm = mix(norm1, norm2, gl_TessCoord.y);
	
	/* not displacing yet */
	vec4 pos1 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 pos2 = mix(gl_in[3].gl_Position, gl_in[2].gl_Position, gl_TessCoord.x);
	_pos = mix(pos1, pos2, gl_TessCoord.y).xyz;

	gl_Position = mvp * mix(pos1, pos2, gl_TessCoord.y);
}