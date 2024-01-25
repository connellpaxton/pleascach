#include <Memory/Buffer.hpp>
#include <Memory/Memory.hpp>

Buffer::Buffer(vk::PhysicalDevice phys_dev, vk::Device dev, vk::DeviceSize sz, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags mem_flags, vk::SharingMode sharing) : dev(dev), size(sz) {
	auto info = vk::BufferCreateInfo {
		.size = sz,
		.usage = usage,
		.sharingMode = sharing,
	};
	buffer = dev.createBuffer(info);

	auto reqs = dev.getBufferMemoryRequirements(buffer);
	auto alloc_info = vk::MemoryAllocateInfo{
		.allocationSize = reqs.size,
		.memoryTypeIndex = mem::choose_heap(phys_dev, reqs, mem_flags),
	};
	memory = dev.allocateMemory(alloc_info);
}

void Buffer::upload(const uint8_t* data, vk::DeviceSize size) {
	auto p = dev.mapMemory(memory, 0, size);
	std::memcpy(p, data, size);
	dev.unmapMemory(memory);
}

Buffer::~Buffer() {
	dev.freeMemory(memory);
	dev.destroyBuffer(buffer);
}

