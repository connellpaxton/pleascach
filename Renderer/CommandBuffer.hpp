#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <util/int.hpp>


struct Buffer;
struct Image;
struct GraphicsPipeline;
struct ComputePipeline;

struct CommandBuffer {
	CommandBuffer(vk::Device dev, u32 queue_family);

	void cleanup(vk::Device dev);

	/* start recording commands */
	void begin();

	/* copy between buffer */
	void copy(vk::Buffer in, vk::Buffer out, vk::ArrayProxy<const vk::BufferCopy> regions);
	/* copy buffer to image */
	void copy(Buffer& in, Image& out, vk::ImageLayout layout = vk::ImageLayout::eTransferDstOptimal);

	void bind(const GraphicsPipeline& pipeline);
	void bind(const ComputePipeline& pipeline);

	/* stop recording commands */
	void end();

	void recycle();

	vk::CommandBuffer command_buffer;
	vk::CommandPool command_pool;

	operator vk::CommandBuffer* () {
		return &command_buffer;
	}
};
