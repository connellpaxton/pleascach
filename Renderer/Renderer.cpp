#include <Renderer/Renderer.hpp>
#include <Window/Window.hpp>

#include <iostream>

Renderer::Renderer(Window* win) : win(win) {
	vkb::InstanceBuilder builder;
	auto inst_ret = builder
		.use_default_debug_messenger()
		.request_validation_layers()
		.build();

	if (!inst_ret) {
		throw "Failed to build vkb instance"s;
	}

	instance = inst_ret.value();

	VkResult res = VK_ERROR_UNKNOWN;
	res = glfwCreateWindowSurface(instance, win->win, NULL, &surface);
	if (res != VK_SUCCESS) {
		throw "Failed to build surface"s;
	}

	vkb::PhysicalDeviceSelector phys_dev_selector(instance, surface);
	/* set preference to discrete */
	phys_dev_selector.prefer_gpu_device_type();

	auto phys_ret = phys_dev_selector.select();
	if (!phys_ret) {
		throw "Failed to select physical device"s;
	}
	phys_dev = phys_ret.value();
	std::cerr << "Selected device: " << phys_dev.name << std::endl;

	/* build device */
	auto dev_ret = vkb::DeviceBuilder(phys_dev).build();
	if (!dev_ret) {
		throw "Failed to create device"s;
	}

	dev = dev_ret.value();

	/* get queue */
	auto queue_ret = dev.get_queue(vkb::QueueType::graphics);
	if (!queue_ret.has_value()) {
		throw "Failed to get graphics queue"s;
	}
	queue = queue_ret.value();

	/* initialize VMA */
	VmaAllocatorCreateInfo create_info{
		.physicalDevice = phys_dev,
		.device = dev,
		.instance = instance,
	};

	if (vmaCreateAllocator(&create_info, &allocator) != VK_SUCCESS) {
		throw "Failed to create allocator"s;
	}

	swapchain = std::make_unique<Swapchain>(this);

	/* create command buffer */
	command_buffer = std::make_unique<CommandBuffer>(dev);
}

Renderer::~Renderer() {

}