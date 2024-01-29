#version 460 core

layout (location = 0) in vec2 texCoord;
layout (location = 0) out vec4 FragColor;

layout (set = 0, binding = 0) uniform Matrices {
	mat4 mpv;
	float time;
	float aspect_ratio;
};

/* 4-float alignment in memory*/
struct Ray {
	vec3 orig;
	float min;
	vec3 dir;
	float max;
};

vec3 ray_color(vec3 ray) {
	float a = 0.5 * (ray.y+1.0);

	return vec3(1.0-a) + a*vec3(0.5, 0.7, 1.0);
}

/* returns float of where you hit, or -1.0 */
float sphere(vec3 center, float r, Ray ray) {
	vec3 p = ray.orig-center;
	float a = dot(ray.dir, ray.dir);
	float half_b = dot(p, ray.dir);
	float c = dot(p, p) - r*r;
	float discr = half_b*half_b - a*c;

	if(discr < 0.0)
		return -1.0;
	
	return -half_b-sqrt(discr)/a;
} 

void main() {
	vec2 pixel = texCoord*2.0-1.0;
	Ray ray;
	ray.orig = vec3(0.0);
	ray.min = 0.1;
	ray.max = 10000.0;
	float fov = radians(90);
	ray.dir = normalize(vec3(pixel.x * aspect_ratio, pixel.y, tan(fov/2.0)));

	vec3 sphere_center = vec3(0.0, 0.0, 1.0);

	float d = sphere(sphere_center, 0.5, ray);

	if(d < 0.0) {
		FragColor = vec4(0.0);
		return;
	}

	vec3 n = normalize(ray.dir*d-sphere_center);
	FragColor = vec4((n+vec3(1.0)) / 2.0, 1.0);

}