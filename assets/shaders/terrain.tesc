#version 450 core
#extension GL_EXT_debug_printf : enable

layout (set = 0, binding = 0) uniform Matrices {
	mat4 view;
	mat4 proj;
	float time;
	vec3 cam_pos;
	vec4 frustum[6];
	vec2 viewport;
	float tess_factor;
	float tess_edge_size;
};

layout(set = 0, binding = 1) uniform sampler2D heightmap;

layout (vertices = 4) out;

layout (location = 0) in vec3 norm[];
layout (location = 1) in vec2 texCoord[];
layout (location = 2) in vec3 pos[];

layout (location = 0) out vec3 _norm[4];
layout (location = 1) out vec2 _texCoord[4];
layout (location = 2) out vec3 _pos[4];


float screen_space_tess(vec4 p0, vec4 p1) {
	/* calc midpoint + distance btw the two points */
	vec4 midp = 0.5*(p0+p1);
	float r = distance(p0, p1) / 2.0;

	vec4 v0 = view * midp;

	/* project into clip spaace */
	vec4 clip0 = proj  * (v0 - vec4(r, vec3(0.0)));
	vec4 clip1 = proj  * (v0 + vec4(r, vec3(0.0)));

	clip0 /= clip0.w;
	clip1 /= clip1.w;

	/* convert to viewport coords */
	clip0.xy *= viewport;
	clip1.xy *= viewport;

	return clamp(distance(clip0, clip1) / tess_edge_size * tess_factor, 1.0, 64.0);
}

bool frustum_culling() {
	/* square root of patch size */
	const float r = 8.0f;
	/* ensure this is consistent with tese shader */
	vec4 fpos = gl_in[gl_InvocationID].gl_Position;
	fpos.y += 15.0 * textureLod(heightmap, texCoord[0], 0.0).r;
	
	for(int i = 0; i < 6; i++) {
		if(dot(fpos, frustum[i]) + r < 0.0)
			return false;
	}
	return true;
}

void main() {

	if(gl_InvocationID == 0) {
		/* perform frustum culling */
		if(frustum_culling()) {
			gl_TessLevelOuter[0] = screen_space_tess(gl_in[3].gl_Position, gl_in[0].gl_Position);
			gl_TessLevelOuter[1] = screen_space_tess(gl_in[0].gl_Position, gl_in[1].gl_Position);
			gl_TessLevelOuter[2] = screen_space_tess(gl_in[1].gl_Position, gl_in[2].gl_Position);
			gl_TessLevelOuter[3] = screen_space_tess(gl_in[2].gl_Position, gl_in[3].gl_Position);
			gl_TessLevelInner[0] = mix(gl_TessLevelOuter[0], gl_TessLevelOuter[3], 0.5);
			gl_TessLevelInner[1] = mix(gl_TessLevelOuter[2], gl_TessLevelOuter[1], 0.5);
		} else {
			gl_TessLevelOuter[0] = 0.0;
			gl_TessLevelOuter[1] = 0.0;
			gl_TessLevelOuter[2] = 0.0;
			gl_TessLevelOuter[3] = 0.0;
			gl_TessLevelInner[0] = 0.0;
			gl_TessLevelInner[1] = 0.0;
		}
	}

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	_norm[gl_InvocationID] = norm[gl_InvocationID];
	_texCoord[gl_InvocationID] = texCoord[gl_InvocationID];
	_pos[gl_InvocationID] = pos[gl_InvocationID];
}