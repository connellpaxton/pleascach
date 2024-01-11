#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <vma/vk_mem_alloc.h>

struct Texture {
	Texture(vkb::Device& dev, VmaAllocator& allocator, const std::string& fname);

	VmaAllocation alloc;
	const std::string fname;
	vk::Image image;
	vk::DescriptorPool desc_pool;
	vk::DescriptorSetLayout layout;
};