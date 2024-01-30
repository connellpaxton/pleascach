#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using glm::sin;
using glm::cos;

struct Camera {
	/* using the math convention of theta = azimuthal */
	float theta = glm::radians(90.0f);
	float phi = 0.0f;
	/* r always = 1 */
	
	glm::vec3 pos = glm::vec3(0.0f);

	inline glm::mat4 view() {
		return glm::lookAt(pos, pos+glm::vec3(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi)), glm::vec3(0.0, 1.0, 0.0));
	}
};