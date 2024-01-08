#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <vkb/VkBootstrap.h>

struct CommandBuffer {
	vk::CommandBuffer command_buffer;
	vk::CommandPool pool;
	vkb::Device& dev;

	CommandBuffer(vkb::Device& dev);
	~CommandBuffer();
};