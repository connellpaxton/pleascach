#version 450 core

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

layout (location = 0) in vec3 norm[];
layout (location = 1) in vec2 texCoord[];
layout (location = 2) in vec3 pos[];

layout (location = 0) out vec3 _norm;
layout (location = 1) out vec2 _texCoord;
layout (location = 2) out vec3 _pos;

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


void main(void) {
	for(int i = 0; i < gl_in.length(); i++) {
		gl_Position = proj * view * gl_in[i].gl_Position;
		_norm = norm[i];
		_texCoord = texCoord[i];
		_pos = pos[i];
		EmitVertex();
	}
	EndPrimitive();
}