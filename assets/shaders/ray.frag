#version 450 core

#define MAX_STEPS 64

const vec2 eps = vec2(0.001, 0.0);

layout (set = 0, binding = 1) uniform sampler2D tex; 

layout (set = 0, binding = 0) uniform Matrices {
	vec3 cam_pos;
	float time;
	vec4 viewport;
	vec3 cam_dir;
	uint n_objects;
}; 

/*
enum Shape {
	SPHERE,
	BOX,
	PLANE,
};
*/

#define SPHERE 0
#define BOX 1
#define PLANE 2

struct Object {
	vec4 center;
	vec4 dimensions;
	uint id;
	uint shape;
};

layout (set = 0, binding = 2) readonly buffer Objects {
	Object objects[];
};

layout (location = 0) in vec2 pos;
layout (location = 0) out vec4 fragColor;

/* joins two parts of a scene */
float op_union(float v1, float v2) {
	return min(v1, v2);
}

/* subtracts sdf from scene */
float op_subtract(float v1, float v2) {
	return max(v1, -v2);
}

float sphere(vec3 p, vec3 c, float r) {
	return length(p-c) - r;
}

float box(vec3 p, vec3 c, vec3 r) {
	p -= c;
	vec3 q = abs(p) - r;
	return length(max(q,0.0)) + min(max(q.x, max(q.y,q.z)), 0.0);
}

float plane(vec3 p, vec3 norm, float d) {
	return dot(p, norm) + d;
}

float obj_to_sdf(vec3 p, uint n) {
	switch(objects[n].shape) {
		case SPHERE:
			return sphere(p, objects[n].center.xyz, objects[n].dimensions.x);
		break;
		case BOX:
			return box(p, objects[n].center.xyz, objects[n].dimensions.xyz);
		break;
	}
}

float map(vec2 p) {
	return sin(p.x)*sin(p.y);
}

float terrain(vec3 p) {
	/* render terrain with bumps */
	return box(p, vec3(0.0), vec3(100.0, 0.1, 100.0)) - abs(map(p.xz));
}

float sdf(vec3 pos) {
	/*float d = 100000000.0;
	for(uint i = 0; i < n_objects; i++) {
		d = op_union(d, obj_to_sdf(pos, i));
	}*/

	/*float dsphere = sphere(pos, vec3(0.0, 3.0*sin(time)-10.0, 0.0), 1.0);
	float dbox = box(pos, vec3(0.0, -10.0, 0.0), vec3(10.0, 1.0, 10.0));
	float dpellet = sphere(pos, vec3(0.0, 10.0*sin(time) - 10.0, 0.0), 0.1);
	float d = op_union(op_subtract(dbox, dsphere), dpellet);*/


	return terrain(pos);
}

float raycast(vec3 dir) {
	float t = 0.0;
	for(int i = 0; i < MAX_STEPS; i++) {
		float dt = sdf(cam_pos + dir * t);
		if(dt < 0.0001*t)
			return float(i)/MAX_STEPS;
		else if(dt > 200.0)
			return -1.0;
		t += dt;
	}

	return -1.0;
}

vec3 norm(vec3 pos) {
	return normalize(
		vec3(
			sdf(pos+eps.xyy),
			sdf(pos+eps.yxy),
			sdf(pos+eps.yyx)
		) - sdf(pos)
	);
}


vec3 raygen() {
	vec3 forward = cam_dir;
	vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
	vec3 up = normalize(cross(right, forward));

	return normalize(pos.x * right + pos.y * up + forward * 2.0);
}

void main() {
	vec3 dir = raygen();
	float d = raycast(dir);

	if(d < 0.0)
		fragColor = vec4(0.0);
	else
		fragColor = vec4(abs(norm(d*dir+cam_pos)).r);
}