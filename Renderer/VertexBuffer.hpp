#pragma once

#include <Memory/Buffer.hpp>

#include <memory>

#include <glm/glm.hpp>

#include <tinygltf/tiny_gltf.h>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 uv;
	glm::vec3 color;
};

struct VertexBuffer {
	std::unique_ptr<Buffer> buffer;


	VertexBuffer(vk::PhysicalDevice phys_dev, vk::Device dev, size_t n_vertices);

	void upload(const std::vector<Vertex>& data);
	void upload(const tinygltf::Buffer& buff, const tinygltf::BufferView& view);
	
	inline vk::VertexInputBindingDescription binding(uint32_t binding, vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eVertex) const {
		return vk::VertexInputBindingDescription {
			.binding = binding,
			.stride = sizeof(Vertex),
			.inputRate = vk::VertexInputRate::eVertex,
		};
	}

	inline std::vector<vk::VertexInputAttributeDescription> attrs(uint32_t binding) const {
		return std::vector<vk::VertexInputAttributeDescription> {
			{
				.location = 0,
				.binding = binding,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = offsetof(Vertex, pos),
			}, {
				.location = 1,
				.binding = binding,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = offsetof(Vertex, norm),
			},{
				.location = 2,
				.binding = binding,
				.format = vk::Format::eR32G32Sfloat,
				.offset = offsetof(Vertex, uv),
			},{
				.location = 3,
				.binding = binding,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = offsetof(Vertex, color),
			}
		};
	}
};