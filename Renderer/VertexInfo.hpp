#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>

struct VertexComponent {
	enum Type {
		f32 = vk::Format::eR32Sfloat,
		vec2 = vk::Format::eR32G32Sfloat,
		vec3 = vk::Format::eR32G32B32Sfloat,
	} type;

	long int location = -1;
	long int offset = -1;

	const inline size_t size() const {
		switch (type) {
			case f32:
				return sizeof(float);
			case vec2:
				return sizeof(glm::vec2);
			case vec3:
				return sizeof(glm::vec3);
		}
	}
};

struct VertexInfo {
	std::vector<VertexComponent> components;

	std::vector<vk::VertexInputAttributeDescription> create_descs() const;

	const inline size_t size() const {
		size_t ret = 0;
		for (const auto& c : components)
			ret += c.size();
		return ret;
	}
};
