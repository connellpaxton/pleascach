#include <Memory/Buffer.hpp>
#include <Memory/Memory.hpp>

#include <util/log.hpp>

Buffer::Buffer(vk::PhysicalDevice phys_dev, vk::Device dev, vk::DeviceSize sz, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags mem_flags, vk::SharingMode sharing) : phys_dev(phys_dev), dev(dev), size(sz) {
	auto info = vk::BufferCreateInfo {
		.size = sz,
		.usage = usage,
		.sharingMode = sharing,
	};
	buffer = dev.createBuffer(info);

	Log::debug("Is memory flagged for host: %d\n", mem_flags & vk::MemoryPropertyFlagBits::eHostVisible);

	auto reqs = dev.getBufferMemoryRequirements(buffer);
	auto alloc_info = vk::MemoryAllocateInfo{
		.allocationSize = reqs.size,
		.memoryTypeIndex = mem::choose_heap(phys_dev, reqs, mem_flags),
	};
	memory = dev.allocateMemory(alloc_info);

	dev.bindBufferMemory(buffer, memory, 0);

	p = dev.mapMemory(memory, 0, size);

	Log::debug("Buffer %p allocated\n", buffer);
}

void Buffer::upload(const uint8_t* data, vk::DeviceSize size) {
	std::memcpy(p, data, size);
}

Buffer::~Buffer() {
	dev.unmapMemory(memory);
	dev.freeMemory(memory);
	dev.destroyBuffer(buffer);
}

