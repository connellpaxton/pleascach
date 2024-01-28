#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <util/log.hpp>

struct Image {
	vk::Device dev;
	vk::Extent3D extent;
	vk::Format format;
	vk::Image image;
	vk::DeviceMemory memory;

	Image(vk::PhysicalDevice phys_dev, vk::Device dev, vk::Image image, vk::MemoryPropertyFlags memory_flags, vk::Extent3D& extent);
	Image(vk::PhysicalDevice phys_dev, vk::Device dev, vk::ImageCreateInfo info, vk::MemoryPropertyFlags memory_flags);
	Image(vk::PhysicalDevice phys_dev, vk::Device dev, vk::Extent3D extent, vk::Format format, vk::ImageTiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memory_flags);

	operator vk::Image&() {
		return image;
	}

	void cleanup();
};
