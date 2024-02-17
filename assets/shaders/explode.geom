#version 450 core

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

layout (location = 0) in vec3 norm[];
layout (location = 1) in vec2 texCoord[];

layout (location = 0) out vec3 _norm;
layout (location = 1) out vec2 _texCoord;

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

vec4 explode(vec4 pos, vec3 n) {
	float mag = 2.0;
	vec3 dir = n * (time-3.0)/10.0 * mag;
	return pos + vec4(dir, 0.0);
}

void main(void) {
	if (time < 3.0) {
		for(int i = 0; i < gl_in.length(); i++) {
			gl_Position = proj * view * gl_in[i].gl_Position;
			_norm = norm[i];
			_texCoord = texCoord[i];
			EmitVertex();
		}
		EndPrimitive();
		return;
	}
	vec3 n = norm[0] + norm[1] + norm[2];
	n/=3;
	for(int i = 0; i < gl_in.length(); i++) {
		gl_Position = proj * view * explode(gl_in[i].gl_Position, n);
		_texCoord = texCoord[i];
		_norm = n;
		EmitVertex();
	}
	EndPrimitive();
}