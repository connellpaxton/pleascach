#version 450 core

layout (set = 0, binding = 0) uniform Matrices {
	vec2 viewport;
	float time;
	vec3 cam_pos;
	vec3 cam_dir;
};

layout (location = 0) in vec2 texCoord;

layout (location = 0) out vec4 fragColor;

void main() {
	fragColor = vec4(abs(texCoord), abs(cos(time)), 1.0);	
}