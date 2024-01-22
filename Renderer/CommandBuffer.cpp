#include <Renderer/CommandBuffer.hpp>
#include <Renderer/Pipeline.hpp>


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

void CommandBuffer::copy(vk::Buffer in, vk::Buffer out, vk::ArrayProxy<const vk::BufferCopy> regions) {
	command_buffer.copyBuffer(in, out, regions);
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
