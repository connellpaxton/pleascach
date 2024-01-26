#include <Memory/Buffer.hpp>

#include <memory>

#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 coord;
};

struct VertexBuffer {
	std::unique_ptr<Buffer> buffer;

	VertexBuffer(vk::PhysicalDevice phys_dev, vk::Device dev, size_t n_vertices);

	void upload(const std::vector<Vertex>& data);
	
	inline vk::VertexInputBindingDescription binding(uint32_t binding, vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eVertex) const {
		return vk::VertexInputBindingDescription {
			.binding = binding,
			.stride = sizeof(Vertex),
			.inputRate = vk::VertexInputRate::eVertex,
		};
	}

	inline std::vector<vk::VertexInputAttributeDescription> attrs(uint32_t binding) const {
		return std::vector<vk::VertexInputAttributeDescription> {
			vk::VertexInputAttributeDescription {
				.location = 0,
				.binding = binding,
				.format = vk::Format::eR32G32B32Sfloat,
				.offset = offsetof(Vertex, coord),
			}
		};
	}
};