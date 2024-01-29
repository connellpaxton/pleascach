#include <Renderer/VertexBuffer.hpp>

#include <Memory/Buffer.hpp>

#include <tinygltf/tiny_gltf.h>

VertexBuffer::VertexBuffer(vk::PhysicalDevice phys_dev, vk::Device dev, size_t n_vertices) {
	buffer = std::make_unique<Buffer>(phys_dev, dev, n_vertices * sizeof(Vertex),
		vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);
}

void VertexBuffer::upload(const std::vector<Vertex>& data) {
	buffer->upload(reinterpret_cast<const uint8_t*>(data.data()), sizeof(Vertex)*data.size());
}

void VertexBuffer::upload(const tinygltf::Buffer& buff, const tinygltf::BufferView& view) {
	std::memcpy(buffer->p, buff.data.data() + view.byteOffset, view.byteLength);
}
