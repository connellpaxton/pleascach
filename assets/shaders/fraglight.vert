#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec3 norm;
layout (location = 1) out vec2 texCoord;
layout (location = 2) out vec3 pos;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mvp;
	float time;
};

void main() {
  pos = (aPos + (vec3(10.0) * gl_InstanceIndex));
  gl_Position = vec4(pos, 1.0);
  texCoord = aTexCoord;
  norm = aNorm;
}