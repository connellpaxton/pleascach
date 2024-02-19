#pragma once

#include <Memory/Buffer.hpp>

#include <memory>

#include <glm/glm.hpp>

#include <tinygltf/tiny_gltf.h>

struct BasicVertex {
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 uv;
	glm::vec3 color;

	static inline std::vector<vk::VertexInputAttributeDescription> attrs(uint32_t binding) {
		return std::vector<vk::VertexInputAttributeDescription> {
			{
				.location = 0,
				.binding = binding,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = offsetof(BasicVertex, pos),
			}, {
				.location = 1,
				.binding = binding,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = offsetof(BasicVertex, norm),
			}, {
				.location = 2,
				.binding = binding,
				.format = vk::Format::eR32G32Sfloat,
				.offset = offsetof(BasicVertex, uv),
			}, {
				.location = 3,
				.binding = binding,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = offsetof(BasicVertex, color),
			}
		};
	}
};

template<typename Vertex>
struct GeneralVertexBuffer {
	std::unique_ptr<Buffer> buffer;

	GeneralVertexBuffer(vk::PhysicalDevice phys_dev, vk::Device dev, size_t n_vertices) {
		buffer = std::make_unique<Buffer>(phys_dev, dev, n_vertices * sizeof(Vertex),
		vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);
	}

	void upload(const std::vector<Vertex>& data) {
		buffer->upload(reinterpret_cast<const uint8_t*>(data.data()), sizeof(Vertex) * data.size());
	}
	void upload(const tinygltf::Buffer& buff, const tinygltf::BufferView& view) {
		std::memcpy(buffer->p, buff.data.data() + view.byteOffset, view.byteLength);
	}
	
	inline vk::VertexInputBindingDescription binding(uint32_t binding, vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eVertex) const {
		return vk::VertexInputBindingDescription {
			.binding = binding,
			.stride = sizeof(Vertex),
			.inputRate = vk::VertexInputRate::eVertex,
		};
	}

	inline std::vector<vk::VertexInputAttributeDescription> attrs(uint32_t binding) const {
		return Vertex::attrs(binding);
	}
};

using VertexBuffer = GeneralVertexBuffer<BasicVertex>;

