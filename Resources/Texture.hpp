#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Texture {
	vk::Image image;
	vk::DeviceMemory image_alloc;
	Texture(vk::Device dev, vk::PhysicalDevice phys_dev, const std::string& fname);
	void cleanup(vk::Device dev);
};