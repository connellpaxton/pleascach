#include <Renderer/Renderer.hpp>
#include <Window/Window.hpp>

#include <util/log.hpp>

#include <Memory/Memory.hpp>


using namespace std::string_literals;

Renderer::Renderer(Window& win) : win(win) {
	/* Create Instance object */
	auto app_info = vk::ApplicationInfo {
		.pApplicationName = "Pl�ascach Demo",
		.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
		.pEngineName = "Pl�ascach",
		.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
		.apiVersion = VK_API_VERSION_1_1,
	};

	const auto req_extensions = win.requiredExtensions();

	auto extensions = vk::enumerateInstanceExtensionProperties();
	std::vector<const char*> extension_names(extensions.size());
	for (size_t i = 0; i < extensions.size(); i++) {
		extension_names[i] = extensions[i].extensionName;
	}

	Log::info("%u Required extensions:\n", (u32)req_extensions.size());
	for (const auto& ext : req_extensions) {
		Log::info("\t- \"%s\"\n", ext);
	}

	/* query and enable available layers if in DEBUG mode */
#ifdef DEBUG

	auto layers = vk::enumerateInstanceLayerProperties();
	Log::info("%zu available instance layers\n", layers.size());
	for (const auto& layer : layers) {
		Log::info("\t\"%s\"\n", layer.layerName.data());
	}

	const char* my_layers[] = {
//		"VK_LAYER_LUNARG_api_dump",
		"VK_LAYER_KHRONOS_validation",
	};

	auto inst_info = vk::InstanceCreateInfo{
		.pApplicationInfo = &app_info,
		.enabledLayerCount = std::size(my_layers),
		.ppEnabledLayerNames = my_layers,
		.enabledExtensionCount = static_cast<u32>(req_extensions.size()),
		.ppEnabledExtensionNames = req_extensions.data(),
	};

#else
	auto inst_info = vk::InstanceCreateInfo{
		.pApplicationInfo = &app_info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = static_cast<u32>(req_extensions.size()),
		.ppEnabledExtensionNames = req_extensions.data(),
	};
#endif

	instance = vk::createInstance(inst_info);

	/* use instance object to obtain physical device, then logical device */
	auto phys_devs = instance.enumeratePhysicalDevices();

	Log::info("Found %u physical device(s)\n", (u32)phys_devs.size());
	int discrete_idx = -1;
	size_t i = 0;
	for (const auto& phys_dev : phys_devs) {
		auto props = phys_dev.getProperties();
		Log::info("\t%zu. \""s + props.deviceName.operator std::string() + "\"\n", i);
		Log::info("\t\t[" + vk::to_string(props.deviceType) + "]\n");
		if (discrete_idx == -1 && props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			discrete_idx = i;
		i++;
	}
	if (discrete_idx == -1)
		discrete_idx = 0;

	auto& phys_dev = phys_devs[discrete_idx];
	Log::info("Selected device: \"%s\" (#%zu)\n", phys_dev.getProperties().deviceName.data(), discrete_idx);
	
	/* find queue family */
	auto queue_family_props = phys_dev.getQueueFamilyProperties();
	queue_family = -1;
	i = 0;
	Log::info("Device has %zu queue families:\n", queue_family_props.size());
	for (const auto& prop : queue_family_props) {
		Log::info("\t%zu. (n: %u) "s + vk::to_string(prop.queueFlags) + "\n", i, prop.queueCount);
		if (queue_family == -1 && prop.queueFlags & vk::QueueFlagBits::eGraphics)
			queue_family = i;
		i++;
	}
	if (queue_family == -1) {
		Log::error("No graphics queues supported\n");
	}

	Log::info("Selected queue family: %i\n", queue_family);

	float priorities[] = {1.0f};
	auto queue_info = vk::DeviceQueueCreateInfo{
		.queueFamilyIndex = static_cast<u32>(queue_family),
		.queueCount = 1,
		.pQueuePriorities = priorities,
	};

	/* enumerate available device features */
	std::vector<const char*> required_extensions;
	required_extensions.push_back("VK_KHR_swapchain");
	auto dev_extentions = phys_dev.enumerateDeviceExtensionProperties();
	Log::info("%zu available device extensions\n", dev_extentions.size());
	for (const auto& ext : dev_extentions) {
		Log::info("\t\"%s\"\n", ext.extensionName.data());
	}
	
	auto dev_layers = phys_dev.enumerateDeviceLayerProperties();
	Log::info("%zu available device layers\n", dev_layers.size());
	for (const auto& layer : dev_layers) {
		Log::info("\t\"%s\"\n", layer.layerName.data());
	}
	std::vector<const char*> dev_layer_names = {
		"VK_LAYER_KHRONOS_validation"
	};

	auto dev_info = vk::DeviceCreateInfo{
		.flags = vk::DeviceCreateFlagBits(0),
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queue_info,
		.enabledLayerCount = static_cast<u32>(dev_layer_names.size()),
		.ppEnabledLayerNames = dev_layer_names.data(),
		.enabledExtensionCount = static_cast<u32>(required_extensions.size()),
		.ppEnabledExtensionNames = required_extensions.data(),
	};

	dev = phys_dev.createDevice(dev_info);
	Log::info("Successfully created logical device\n");

	/* create swapchain (requires SurfaceKHR from windowing system) */
	surface = win.getSurface(instance);
	if (surface == VK_NULL_HANDLE) {
		Log::error("Failed to get surface from window\n");
	}

	queue = dev.getQueue(queue_family, 0);


	/* depth image */
	/* 1. create an Image to be the buffer
	 * 2. find memory req's
	 * 3. allocate
	 * 4. set layout
	 * 5. create attachment view
	 */
	auto extent = win.getDimensions();
	auto depth_image_info = vk::ImageCreateInfo {
		.imageType = vk::ImageType::e2D,
		.format = vk::Format::eD16Unorm,
		.extent = {
			.width = extent.width,
			.height = extent.height,
			.depth = 1,
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
		.sharingMode = vk::SharingMode::eExclusive,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.initialLayout = vk::ImageLayout::eUndefined,
	};

	depth_image = dev.createImage(depth_image_info);

	auto depth_mem_reqs = dev.getImageMemoryRequirements(depth_image);

	auto depth_alloc_info = vk::MemoryAllocateInfo {
		.allocationSize = depth_mem_reqs.size,
		.memoryTypeIndex = mem::choose_heap(phys_dev, depth_mem_reqs, vk::MemoryPropertyFlagBits::eDeviceLocal),
	};

	depth_alloc = dev.allocateMemory(depth_alloc_info);
	dev.bindImageMemory(depth_image, depth_alloc, 0);

	auto depth_view_info = vk::ImageViewCreateInfo {
		.image = depth_image,
		.viewType = vk::ImageViewType::e2D,
		.format = vk::Format::eD16Unorm,
		.components = {
			.r = vk::ComponentSwizzle::eR,
			.g = vk::ComponentSwizzle::eG,
			.b = vk::ComponentSwizzle::eB,
			.a = vk::ComponentSwizzle::eA,
		},
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eDepth,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};

	depth_image_view = dev.createImageView(depth_view_info);

	render_fence = dev.createFence(vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled });

	image_wait_semaphore = dev.createSemaphore(vk::SemaphoreCreateInfo{});
	render_wait_semaphore = dev.createSemaphore(vk::SemaphoreCreateInfo{});

	render_pass = std::make_unique<RenderPass>(dev);
	swapchain = std::make_unique<Swapchain>(dev, surface, win.getDimensions(), *render_pass, depth_image_view);

	command_buffer = std::make_unique<CommandBuffer>(dev, queue_family);

}

void Renderer::draw() {
	Log::info("draw() called \n");

	dev.waitForFences(render_fence, true, UINT64_MAX);
	dev.resetFences(render_fence);

	/* check if the swapchain is still good (no resize) */
	auto image_ret = dev.acquireNextImageKHR(*swapchain, UINT64_MAX, image_wait_semaphore);
	if (image_ret.result == vk::Result::eErrorOutOfDateKHR) {
		swapchain->recreate(win.getDimensions());
	}

	current_image_idx = image_ret.value;

	/* prepare command buffer for recording commands */
	command_buffer->recycle();
	command_buffer->begin();
	
	vk::ClearValue clear_values[] = {
		vk::ClearColorValue(1.0f, 0.0f, 1.0f, 1.0f),
		vk::ClearDepthStencilValue {.depth = 1.0f}
	};

	/* use renderpass to transform images from unspecified layout to a presentable one while clearing */
	auto render_pass_info = vk::RenderPassBeginInfo {
		.renderPass = *render_pass,
		.framebuffer = swapchain->framebuffers[current_image_idx],
		.renderArea = {
			.offset = { 0, 0 },
			.extent = swapchain->extent,
		},
		.clearValueCount = std::size(clear_values),
		.pClearValues = clear_values,
	};

	auto viewport = vk::Viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<float>(swapchain->extent.width),
		.height = static_cast<float>(swapchain->extent.height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	auto scissor = vk::Rect2D{
		.offset = {0, 0},
		.extent = swapchain->extent,
	};

	
	/* no secondary command buffers (yet), so contents are passed inline */
	command_buffer->command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
	command_buffer->command_buffer.setViewport(0, viewport);
	command_buffer->command_buffer.setScissor(0, scissor);
	command_buffer->command_buffer.endRenderPass();
	
	command_buffer->end();


	vk::PipelineStageFlags stage_flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	/* submit our command buffer to the queue */
	auto submit_info = vk::SubmitInfo{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &image_wait_semaphore,
		.pWaitDstStageMask = &stage_flags,
		.commandBufferCount = 1,
		.pCommandBuffers = &command_buffer->command_buffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &render_wait_semaphore,
	};

	queue.submit(submit_info, render_fence);
}

void Renderer::present() {
	Log::info("present() called \n");
	auto present_info = vk::PresentInfoKHR{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &render_wait_semaphore,
		.swapchainCount = 1,
		.pSwapchains = &swapchain->operator vk::SwapchainKHR & (),
		.pImageIndices = &current_image_idx,
	};

	switch (queue.presentKHR(present_info)) {
		case vk::Result::eSuccess:
		break;
		case vk::Result::eSuboptimalKHR:
		case vk::Result::eErrorOutOfDateKHR:
			Log::info("Recreating swapchain");
			swapchain->recreate(win.getDimensions());
		break;
		default:
			Log::error("Failed to present surface.");
		break;
	}
}

Renderer::~Renderer() {
	dev.destroyImage(depth_image);
	dev.destroyImageView(depth_image_view);
	dev.freeMemory(depth_alloc);


	swapchain.reset();
	dev.waitIdle();
	command_buffer->cleanup(dev);
	render_pass->cleanup(dev);
	dev.destroy();
	instance.destroySurfaceKHR(surface);
	instance.destroy();
}
