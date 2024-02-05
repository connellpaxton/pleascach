#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Buffer {
	Buffer(vk::PhysicalDevice phys_dev, vk::Device dev, vk::DeviceSize sz, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags mem_flags, vk::SharingMode sharing = vk::SharingMode::eExclusive);

	vk::PhysicalDevice phys_dev;
	vk::Device dev;
	vk::DeviceSize size;
	vk::DeviceMemory memory;
	vk::Buffer buffer;
	/* mapped memory */
	void* p;

	void upload(const uint8_t* data, vk::DeviceSize size);
	inline void upload(const uint8_t* data) {
		upload(data, size);
	}
	template<typename T>
	inline void upload(const std::vector<T>& data) {
		upload(reinterpret_cast<const uint8_t*>(data.data()), data.size()*sizeof(T));
	}

	operator vk::Buffer& () {
		return buffer;
	}

	~Buffer();
};
