#pragma once

#include <glm/glm.hpp>
#include <array>

/* extracts frustum from projection matrix */
static std::array<glm::vec4, 6> frustum(const glm::mat4& mat) {
	/* Left, Right, Top, Bottom, Back, Front */
	std::array<glm::vec4, 6> ret;

	for (size_t i = 0; i < 3; i++)
		for (size_t j = 0; j < 4; j++)
			ret[i * 2][j] = mat[j].w + mat[j][i],
			ret[i * 2 + 1][j] = mat[j].w - mat[j][i];

	return ret;
}