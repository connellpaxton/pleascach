#pragma once

#include <Renderer/VertexInfo.hpp>

struct GltfVertex {
	static const VertexInfo Info {
		.componenets = {
			{ .type = VertexComponent::Type::vec3 }
			{ .type = VertexComponent::Type::vec3 }
			{ .type = VertexComponent::Type::vec3 }
		},
	};
};