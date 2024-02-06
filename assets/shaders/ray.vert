#version 460 core

layout (location = 0) in vec2 aCoord;

layout (location = 0) out vec2 texCoord;

layout (set = 0, binding = 0) uniform Matrices {
	vec2 viewport;
	float time;
	vec3 cam_pos;
	vec3 cam_dir;
};

void main() {
	texCoord = aCoord;
	gl_Position = vec4(aCoord, 0.0, 1.0);
}