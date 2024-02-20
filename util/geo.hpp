#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <array>

static glm::vec4 extract_plane(const glm::mat4& mat, int col) {
	float sign;
	if (col < 0) {
		sign = -1.0;
		col = -col - 1;
	} else {
		sign = 1.0;
		col -= 1;
	}

	return glm::column(mat, 3) + sign * glm::column(mat, col);
}

/* extracts frustum from projection matrix */
static std::array<glm::vec4, 6> frustum(const glm::mat4& mat) {
	/* Left, Right, Top, Bottom, Back, Front */
	return std::array<glm::vec4, 6> {
		extract_plane(mat, 1),
		extract_plane(mat, -1),
		extract_plane(mat, 2),
		extract_plane(mat, -2),
		extract_plane(mat, 3),
		extract_plane(mat, -3),
	};

	/*for (size_t i = 0; i < 3; i++)
		for (size_t j = 0; j < 4; j++)
			ret[i * 2][j] = mat[j].w + mat[j][i],
			ret[i * 2 + 1][j] = mat[j].w - mat[j][i];*/

}

static bool box_in_frustum(const std::array<glm::vec4, 6>& frustum, const glm::vec3 box_verts[8]) {
	for (const auto& plane : frustum) {
		for (size_t i = 0; i < 8; i++) {
			const auto& vert = box_verts[i];
			if (glm::dot(glm::vec4(vert, 1.0), plane) > 0)
				goto double_continue;
		}
		return false;
	double_continue:;
	}

	return true;
}