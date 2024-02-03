#version 450 core

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

layout (location = 0) in vec3 norm[];
layout (location = 1) in vec2 texCoord[];

layout (location = 0) out vec3 _norm;
layout (location = 1) out vec2 _texCoord;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mvp;
	float time;
};


void main(void) {
	for(int i = 0; i < gl_in.length(); i++) {
		gl_Position = mvp * gl_in[i].gl_Position;
		_norm = norm[i];
		_texCoord = texCoord[i];
		EmitVertex();
	}
	EndPrimitive();
}