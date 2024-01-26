#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout (location = 0) out vec2 texCoord;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mpv;
	float time;
};

void main() {
  gl_Position = mpv*vec4(aPos, 1.0);
  texCoord = aTexCoord;
}