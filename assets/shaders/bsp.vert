#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec2 lightmapCoord;
layout (location = 3) in vec3 aNorm;
layout (location = 4) in uint aColor;

layout (location = 0) out vec3 norm;
layout (location = 1) out vec2 texCoord;
layout (location = 2) out vec4 color;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 view;
	mat4 proj;
	float time;
	vec3 cam_pos;
	vec3 cam_dir;
	vec4 frustum[6];
	vec2 viewport;
	float tess_factor;
	float tess_edge_size;
};

vec4 unpackABGR(uint packedABGR) {
    float scale = 1.0 / 255.0;
    float a = float((packedABGR >> 24) & 0xFF) * scale;
    float b = float((packedABGR >> 16) & 0xFF) * scale;
    float g = float((packedABGR >> 8) & 0xFF) * scale;
    float r = float(packedABGR & 0xFF) * scale;
    return vec4(r,g,b,a);
}


void main() {
  mat4 zup_to_yup = mat4(
    1.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 1.0
  );

  gl_Position = proj * view * zup_to_yup * vec4(aPos, 1.0);
  texCoord = aTexCoord;
  norm = aNorm;
  color = unpackABGR(aColor);
}