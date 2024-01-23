#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Buffer {
	Buffer(vk::PhysicalDevice phys_dev, vk::Device dev, vk::DeviceSize sz, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags mem_flags, vk::SharingMode sharing = vk::SharingMode::eExclusive);

	vk::DeviceSize size;
	vk::Device dev;
	vk::DeviceMemory memory;
	vk::Buffer buffer;

	void map(const uint8_t* data, vk::DeviceSize size);
	inline void map(const uint8_t* data) {
		map(data, size);
	}
	inline void map(const std::vector<uint8_t>& data) {
		map(data.data(), size);
	}

	operator vk::Buffer& () {
		return buffer;
	}

	~Buffer();
};
