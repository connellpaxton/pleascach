#include <renderer/command_buffer.hpp>

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

	dev.allocateCommandBuffers(alloc_info);
}

void CommandBuffer::begin() {
	auto begin_info = vk::CommandBufferBeginInfo{
		.flags = static_cast<vk::CommandBufferUsageFlags>(0),
		.pInheritanceInfo = nullptr,
	};

	command_buffer.begin(begin_info);
}

void CommandBuffer::copy(vk::Buffer in, vk::Buffer out, vk::ArrayProxy<const vk::BufferCopy> regions) {
	command_buffer.copyBuffer(in, out, regions);
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
