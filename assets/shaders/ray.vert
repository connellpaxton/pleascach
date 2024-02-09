#version 460 core

layout (location = 0) in vec2 aCoord;

layout (location = 0) out vec2 pos;

layout (set = 0, binding = 0) uniform Matrices {
	vec3 cam_pos;
	float time;
	vec4 viewport;
	vec3 cam_dir;
}; 

void main() {
	gl_Position = vec4(aCoord, 0.0, 1.0);
	pos = vec2(aCoord.x * viewport.x / viewport.y, aCoord.y);
}