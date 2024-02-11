#version 450 core

#define MAX_STEPS 100
#define MAX_DIST 1000.0

layout (set = 0, binding = 0) uniform Matrices {
	vec3 cam_pos;
	float time;
	vec4 viewport;
	vec3 cam_dir;
	uint n_objects;
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

float box(vec3 p, vec3 r) {
	vec3 q = abs(p) - r;
	return length(max(q,0.0)) + min(max(q.x, max(q.y,q.z)), 0.0);
}

/* square, centered at the origin */
float box(vec2 p, vec2 r) {
	vec2 d = abs(p)-r;
	return length(max(d, 0.0)) + min(max(d.x,d.y), 0.0);
}

/* infinite cross, used for construction of serpinski cube */
float infcross(vec3 p, float r) {
	vec2 unit = vec2(r);
	float box1 = box(p.xy, unit);
	float box2 = box(p.yz, unit);
	float box3 = box(p.zx, unit);
	return op_union(op_union(box1, box2), box3);
}

float sphere(vec3 p, float r) {
	return length(p) - r;
}

float sdf(vec3 p) {
	float d = box(p, vec3(1.0));
	float s = 1.0;
	for(int i = 0; i < 5; i++) {
		/* using mod to repeat across domain, then shift to +- */
		vec3 a = mod(p * s, 2.0) - 1.0;
		s *= 3.0;
		vec3 r = 1.0 - 3.0*abs(a);
		float c = infcross(r, 1.0)/s;
		d = max(d, c);
	}
	return d;
}

vec2 raycast(vec3 dir) {
	float t = 0.0;
	for(int i = 0; i < MAX_STEPS; i++) {
		float dt = sdf(cam_pos + dir * t);
		if(dt < 0.0001*t)
			return vec2(t, float(i));
		/*else if(dt > MAX_DIST)
			return vec2(t, float(i));*/
		t += dt;
	}

	return vec2(0, 64.0);
}

vec3 raygen() {
	vec3 forward = cam_dir;
	vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
	vec3 up = normalize(cross(right, forward));

	return normalize(pos.x * right + pos.y * up + forward * 2.0);
}

void main() {
	vec3 dir = raygen();
	vec2 d = raycast(dir);
	vec3 point = cam_pos + dir * d.x;

	fragColor = vec4(d.y/MAX_STEPS);
}