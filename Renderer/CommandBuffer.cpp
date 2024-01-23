#include <Renderer/CommandBuffer.hpp>
#include <Renderer/Pipeline.hpp>

#include <Memory/Buffer.hpp>
#include <Memory/Image.hpp>

#include <util/log.hpp>

CommandBuffer::CommandBuffer(vk::Device dev, u32 queue_family) {
	/* (For now) allow command buffers to be individually recycled */
	auto pool_info = vk::CommandPoolCreateInfo {
		.flags	= vk::CommandPoolCreateFlagBits::eResetCommandBuffer
				| vk::CommandPoolCreateFlagBits::eTransient,
		.queueFamilyIndex = queue_family,
	};

	command_pool = dev.createCommandPool(pool_info);

	/* for now, only using primary level command buffers */
	auto alloc_info = vk::CommandBufferAllocateInfo{
		.commandPool = command_pool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1,
	};
	
	command_buffer = dev.allocateCommandBuffers(alloc_info)[0];
}

void CommandBuffer::begin() {
	auto begin_info = vk::CommandBufferBeginInfo{
		.flags = static_cast<vk::CommandBufferUsageFlags>(0),
	};

	command_buffer.begin(begin_info);
}

void CommandBuffer::copy(vk::Buffer src, vk::Buffer dst, vk::ArrayProxy<const vk::BufferCopy> regions) {
	command_buffer.copyBuffer(src, dst, regions);
}

void CommandBuffer::copy(Buffer& src, Image& dst, vk::ImageLayout layout) {
	auto region = vk::BufferImageCopy{
		.bufferOffset = 0,
		/* RowLength and ImageHeight are automatically set to imageExtent dimensions if set to 0 */
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.mipLevel = 0,
			.baseArrayLayer = 0,
			.layerCount = 0,
		},
		.imageOffset = { 0, 0, 0 },
		.imageExtent = dst.extent,
	};

	command_buffer.copyBufferToImage(src, dst.image, layout, region);
}

void CommandBuffer::bind(const GraphicsPipeline& pipeline) {
	command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline);
}

void CommandBuffer::bind(const ComputePipeline& pipeline) {
	command_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline.pipeline);
}

void CommandBuffer::end() {
	command_buffer.end();
}

void CommandBuffer::recycle() {
	command_buffer.reset();

}

void CommandBuffer::cleanup(vk::Device dev) {
	dev.freeCommandBuffers(command_pool, command_buffer);
	dev.destroyCommandPool(command_pool);
}