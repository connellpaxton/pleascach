#version 450 core

layout (set = 0, binding = 0) uniform Matrices {
	mat4 view;
	vec2 viewport;
	float time;
};

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 dir;

layout (location = 0) out vec4 fragColor;


float sphere(vec3 p, vec3 center, float r) {
	return length(p) - r;
}

float sdf(vec3 pos) {
	return sphere(pos, vec3(0.0, 0.0, 10.0), 3.0);
}

float raycast() {
	float t = 0.0;
	for(int i = 0; i < 64; i++) {
		float dt = sdf(pos + dir * t);
		if(dt < 0.0001*t)
			return t;
		else if(dt > 200.0)
			return -1;
		t += dt;
	}

	return -1;
}

void main() {
	float d = raycast();

	vec3 col = vec3(1.0-d*0.075);

	fragColor = vec4(col, 1.0);
}