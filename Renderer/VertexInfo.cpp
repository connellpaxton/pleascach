#pragma once

#include <Renderer/VertexInfo.hpp>

std::vector<vk::VertexInputAttributeDescription> VertexInfo::create_descs() const {
	std::vector<vk::VertexInputAttributeDescription> ret;

	unsigned i = 0;
	uint32_t offset = 0;
	for (const auto& component : components) {
		ret.push_back(
			vk::VertexInputAttributeDescription {
				.location = (component.location == -1) ? i : component.location,
				.binding = 0,
				.format = static_cast<vk::Format>(component.type),
				.offset = (component.offset == -1) ? offset : component.offset,
			}
		);
		i++;
		offset += component.size();
	}

	return ret;
}