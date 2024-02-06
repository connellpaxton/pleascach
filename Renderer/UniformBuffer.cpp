#include <Renderer/UniformBuffer.hpp>

#include <util/log.hpp>

UniformBuffer::UniformBuffer(vk::PhysicalDevice phys_dev, vk::Device dev) {
	buffer = std::make_unique<Buffer>(phys_dev, dev, sizeof(UniformData),
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
	);

	Log::debug("Offset of time: %zu\n", offsetof(UniformData, time));
	Log::debug("Offset of cam_pos: %zu\n", offsetof(UniformData, cam_pos));
	Log::debug("Offset of viewport: %zu\n", offsetof(UniformData, viewport));

}

void UniformBuffer::upload(const UniformData& data) {
	buffer->upload(reinterpret_cast<const uint8_t*>(&data), sizeof(UniformData));
}