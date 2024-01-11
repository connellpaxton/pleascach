#include <Renderer/CommandBuffer.hpp>

CommandBuffer::CommandBuffer(vkb::Device& dev) : dev(dev) {
	/* create pool */
	vk::CommandPoolCreateInfo pool_info {
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = dev.get_queue_index(vkb::QueueType::graphics).value(),
	};

	pool = vk::Device(dev).createCommandPool(pool_info);

	vk::CommandBufferAllocateInfo buffer_alloc_info {
		.commandPool = pool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1,
	};

	buffer = vk::Device(dev).allocateCommandBuffers(buffer_alloc_info).front();
}

CommandBuffer::~CommandBuffer() {
	vk::Device(dev).freeCommandBuffers(pool, 1, &buffer);
	vk::Device(dev).destroyCommandPool(pool);
}