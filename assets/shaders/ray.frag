#version 450 core

layout (set = 0, binding = 0) uniform Matrices {
	vec3 cam_pos;
	float time;
	vec4 viewport;
	vec3 cam_dir;
}; 

layout (location = 0) in vec2 pos;
layout (location = 0) out vec4 fragColor;

float opUnion(float v1, float v2) {
	return min(v1, v2);
}

float box(vec3 p, vec3 c, vec3 r) {
	p -= c;
	vec3 q = abs(p) - r;
	return length(max(q,0.0)) + min(max(q.x, max(q.y,q.z)), 0.0);
}

float sphere(vec3 p, vec3 c, float r) {
	return length(p-c) - r;
}

float sdf(vec3 pos) {
	float d = sphere(pos, vec3(0.0, 0.0, 0.0), 3.0);
	d = opUnion(d, box(pos, vec3(0.0, -10.0, 0.0), vec3(10.0, 1.0, 10.0)));
	return d;
}

float raycast(vec3 dir) {
	float t = 0.0;
	for(int i = 0; i < 64; i++) {
		float dt = sdf(cam_pos + dir * t);
		if(dt < 0.0001*t)
			return t;
		else if(dt > 200.0)
			return -1.0;
		t += dt;
	}

	return -1.0;
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
	vec3 point = cam_pos + dir * d;

	if(d < 0.0)
		fragColor = vec4(0.0);
	else
		fragColor = vec4(mod(point.y, 1.0));
}