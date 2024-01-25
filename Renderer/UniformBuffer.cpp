#include <Renderer/UniformBuffer.hpp>

UniformBuffer::UniformBuffer(vk::PhysicalDevice phys_dev, vk::Device dev) {
	buffer = std::make_unique<Buffer>(phys_dev, dev, sizeof(UniformData),
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
	);
}

void UniformBuffer::upload(const UniformData& data) {
	buffer->upload(reinterpret_cast<const uint8_t*>(&data), sizeof(UniformData));
}