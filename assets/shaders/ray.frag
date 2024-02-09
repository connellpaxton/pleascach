#version 450 core

layout (set = 0, binding = 0) uniform Matrices {
	vec3 cam_pos;
	float time;
	vec4 viewport;
	vec3 cam_dir;
}; 

layout (location = 0) in vec2 pos;
layout (location = 0) out vec4 fragColor;

float sphere(vec3 p, vec3 center, float r) {
	return length(p-center) - r;
}

float sdf(vec3 pos) {
	return sphere(pos, vec3(0.0, 0.0, 0.0), 3.0);
}

float raycast(vec3 dir) {
	float t = 0.0;
	for(int i = 0; i < 64; i++) {
		float dt = sdf(cam_pos + dir * t);
		if(dt < 0.0001*t)
			return float(i) / 64.0;
		else if(dt > 200.0)
			return 1.0;
		t += dt;
	}

	return 1.0;
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

	fragColor = vec4(d);
}