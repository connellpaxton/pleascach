#version 460 core

layout (location = 0) in vec3 norm;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 pos;
layout (location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mvp;
	float time;
	vec3 cam_pos;
};

layout (set = 0, binding = 1) uniform sampler2D tex;     

void main() {
	vec3 light_pos = normalize(vec3(cos(time), sin(time), 0.0))*10.0;
	/* calculate highlight using angle between fragment and viewer */
	vec3 I = normalize(cam_pos-pos);
	vec3 L = normalize(light_pos-pos);
	vec3 cool = vec3(0.0, 0.0, 0.55);
	vec3 warm = vec3(0.3, 0.3, 0);
	float t = (dot(norm, L)+1.0)/2.0;
	vec3 r = 2.0*(dot(norm,L)*norm)-L;
	float s = 100.0*(dot(r,I))-97.0;
	if(s < 0.0)
		s = 0.0;
	else if(s > 1.0)
		s = 1.0;

	FragColor = vec4((1.0-s)*(t*warm + (vec3(0.5)-t)*cool) + s*vec3(1.0), 1.0);
}	