#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Buffer {
	Buffer(vk::PhysicalDevice phys_dev, vk::Device dev, vk::DeviceSize sz, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags mem_flags, vk::SharingMode sharing = vk::SharingMode::eExclusive);

	vk::Device dev;
	vk::DeviceSize size;
	vk::DeviceMemory memory;
	vk::Buffer buffer;

	void upload(const uint8_t* data, vk::DeviceSize size);
	inline void upload(const uint8_t* data) {
		upload(data, size);
	}
	inline void upload(const std::vector<uint8_t>& data) {
		upload(data.data(), size);
	}

	operator vk::Buffer& () {
		return buffer;
	}

	~Buffer();
};
