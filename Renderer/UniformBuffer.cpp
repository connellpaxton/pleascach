#include <Renderer/UniformBuffer.hpp>

#include <util/log.hpp>

UniformBuffer::UniformBuffer(vk::PhysicalDevice phys_dev, vk::Device dev) {
	buffer = std::make_unique<Buffer>(phys_dev, dev, sizeof(UniformData),
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
	);

	Log::debug("Offset of view: %zu\n", offsetof(UniformData, view));
	Log::debug("Offset of proj: %zu\n", offsetof(UniformData, proj));
	Log::debug("Offset of time: %zu\n", offsetof(UniformData, time));
	Log::debug("Offset of pad0: %zu\n", offsetof(UniformData, pad0));
	Log::debug("Offset of pad1: %zu\n", offsetof(UniformData, pad1));
	Log::debug("Offset of pad2: %zu\n", offsetof(UniformData, pad2));
	Log::debug("Offset of cam_pos: %zu\n", offsetof(UniformData, cam_pos));
	Log::debug("Offset of viewport: %zu\n", offsetof(UniformData, viewport));
	Log::debug("Offset of tess_factor: %zu\n", offsetof(UniformData, tess_factor));
	Log::debug("Offset of tess_edge_size: %zu\n", offsetof(UniformData, tess_edge_size));

}

void UniformBuffer::upload(const UniformData& data) {
	buffer->upload(reinterpret_cast<const uint8_t*>(&data), sizeof(UniformData));
}