#pragma once
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <util/int.hpp>

namespace mem {
	u32 choose_heap(vk::PhysicalDevice& phys_dev, const vk::MemoryRequirements& requirements, vk::MemoryPropertyFlags req_flags, vk::MemoryPropertyFlags pref_flags);
}