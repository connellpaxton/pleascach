#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 14) out;

layout (location = 0) in vec3 mins[];
layout (location = 1) in vec3 maxes[];
layout (location = 2) in uint id[];

layout (location = 0) out uint _id;

layout (set = 0,	binding = 0) uniform Matrices {
	mat4 view;
	mat4 proj;
};


void main(void) {
	int cube_vertices[14] = {
		0, 1, 2, 3, 7, 6, 5, 4, 0, 1, 5, 6, 2, 3
	};

	for(int i = 0; i < gl_in.length(); i++) {
		vec3 vertices[] = {
            vec3(mins[i].x, mins[i].y, mins[i].z),
            vec3(mins[i].x, mins[i].y, maxes[i].z),
            vec3(mins[i].x, maxes[i].y, mins[i].z),
            vec3(mins[i].x, maxes[i].y, maxes[i].z),
            vec3(maxes[i].x, mins[i].y, mins[i].z),
            vec3(maxes[i].x, mins[i].y, maxes[i].z),
            vec3(maxes[i].x, maxes[i].y, mins[i].z),
            vec3(maxes[i].x, maxes[i].y, maxes[i].z),
		};

		_id = id[i];
		for(int j = 0; j < 14; j++) {
			gl_Position = proj*view*vec4(vertices[cube_vertices[j]], 1.0);
			EmitVertex();
		}
		EndPrimitive();
	}
}