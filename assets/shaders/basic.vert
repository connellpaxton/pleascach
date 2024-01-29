#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec3 norm;
layout (location = 1) out vec2 texCoord;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mpv;
	float time;
};

void main() {
  gl_Position = mpv * vec4(aPos - vec3(0.0, 0.0, 3.0) * time/3.0, 1.0);
  texCoord = aTexCoord;
  norm = aNorm;
}