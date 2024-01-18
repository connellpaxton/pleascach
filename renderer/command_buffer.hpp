#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <util/int.hpp>

struct CommandBuffer {
	CommandBuffer(vk::Device dev, u32 queue_family);

	void cleanup(vk::Device dev);

	/* start recording commands */
	void begin();

	/* copy between buffer */
	void copy(vk::Buffer in, vk::Buffer out, vk::ArrayProxy<const vk::BufferCopy> regions);

	/* stop recording commands */
	void end();

	void recycle();

	vk::CommandBuffer command_buffer;
	vk::CommandPool command_pool;
};