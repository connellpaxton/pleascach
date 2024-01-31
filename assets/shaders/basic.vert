#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec3 norm;
layout (location = 1) out vec2 texCoord;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mvp;
	float time;
};

void main() {
  //gl_Position = mvp * vec4(aPos, 1.0);
  gl_Position = vec4(aPos, 1.0);
  texCoord = aTexCoord;
  norm = aNorm;
}