#include <Renderer/Renderer.hpp>
#include <Resources/Texture.hpp>

#include <Window/Window.hpp>

#include <util/log.hpp>

#include <Memory/Memory.hpp>

#include <util/Timer.hpp>
#include <util/geo.hpp>

#include <Renderer/Pipeline.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/UniformBuffer.hpp>
#include <Renderer/VertexBuffer.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <UI/UI.hpp>



using namespace std::string_literals;

Renderer::Renderer(Window& win) : win(win) {
	/* Create Instance object */
	auto app_info = vk::ApplicationInfo{
		.pApplicationName = "Pléascach Demo",
		.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
		.pEngineName = "Pléascach",
		.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
		.apiVersion = VK_API_VERSION_1_1,
	};

	auto req_instance_extensions = win.requiredExtensions();

	auto extensions = vk::enumerateInstanceExtensionProperties();
	std::vector<const char*> extension_names(extensions.size());
	for (size_t i = 0; i < extensions.size(); i++) {
		extension_names[i] = extensions[i].extensionName;
	}

	Log::info("%u Required extensions:\n", (u32)req_instance_extensions.size());
	for (const auto& ext : req_instance_extensions) {
		Log::info("\t- \"%s\"\n", ext);
	}


	/* query and enable available layers if in DEBUG mode */
#ifdef _DEBUG

	auto layers = vk::enumerateInstanceLayerProperties();
	Log::info("%zu available instance layers\n", layers.size());
	for (const auto& layer : layers) {
		Log::info("\t\"%s\"\n", layer.layerName.data());
	}

	vk::ValidationFeatureEnableEXT enabled[] = { vk::ValidationFeatureEnableEXT::eDebugPrintf, vk::ValidationFeatureEnableEXT::eBestPractices };
	vk::ValidationFeaturesEXT validation_features{ };
	validation_features.disabledValidationFeatureCount = 0;
	validation_features.enabledValidationFeatureCount = std::size(enabled);
	validation_features.pDisabledValidationFeatures = nullptr;
	validation_features.pEnabledValidationFeatures = enabled;

	const char* my_layers[] = {
		//		"VK_LAYER_LUNARG_api_dump",
				"VK_LAYER_KHRONOS_validation",
	};

	auto inst_info = vk::InstanceCreateInfo{
		.pNext = &validation_features,
		.pApplicationInfo = &app_info,
		.enabledLayerCount = std::size(my_layers),
		.ppEnabledLayerNames = my_layers,
		.enabledExtensionCount = static_cast<u32>(req_instance_extensions.size()),
		.ppEnabledExtensionNames = req_instance_extensions.data(),
	};

#else
	auto inst_info = vk::InstanceCreateInfo{
		.pApplicationInfo = &app_info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = static_cast<u32>(req_instance_extensions.size()),
		.ppEnabledExtensionNames = req_instance_extensions.data(),
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

	phys_dev = phys_devs[discrete_idx];
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

	float priorities[] = { 1.0f };
	auto queue_info = vk::DeviceQueueCreateInfo{
		.queueFamilyIndex = static_cast<u32>(queue_family),
		.queueCount = 1,
		.pQueuePriorities = priorities,
	};

	std::vector<const char*> req_dev_extensions;
	req_dev_extensions.push_back("VK_KHR_swapchain");
	req_dev_extensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
	req_dev_extensions.push_back("VK_KHR_shader_non_semantic_info");

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

	const auto features = vk::PhysicalDeviceFeatures{
		.geometryShader = vk::True,
		.tessellationShader = vk::True,
		.fillModeNonSolid = vk::True,
	};

	auto dev_info = vk::DeviceCreateInfo{
		.flags = vk::DeviceCreateFlagBits(0),
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queue_info,
		.enabledLayerCount = static_cast<u32>(dev_layer_names.size()),
		.ppEnabledLayerNames = dev_layer_names.data(),
		.enabledExtensionCount = static_cast<u32>(req_dev_extensions.size()),
		.ppEnabledExtensionNames = req_dev_extensions.data(),
		.pEnabledFeatures = &features,
	};

	dev = phys_dev.createDevice(dev_info);
	Log::info("Successfully created logical device\n");

	/* create swapchain (requires SurfaceKHR from windowing system) */
	surface = win.getSurface(instance);
	if (surface == VK_NULL_HANDLE) {
		Log::error("Failed to get surface from window\n");
	}

	queue = dev.getQueue(queue_family, 0);

	render_fence = dev.createFence(vk::FenceCreateInfo {.flags = vk::FenceCreateFlagBits::eSignaled });

	image_wait_semaphore = dev.createSemaphore(vk::SemaphoreCreateInfo{});
	render_wait_semaphore = dev.createSemaphore(vk::SemaphoreCreateInfo{});

	auto color_format = Swapchain::Capabilities(phys_dev, surface).chooseFormat().format;
	render_pass = std::make_unique<RenderPass>(dev, color_format);
	swapchain = std::make_unique<Swapchain>(win, dev, phys_dev, surface, *render_pass);

	command_buffer = std::make_unique<CommandBuffer>(dev, queue_family);

	uniform_buffer = std::make_unique<UniformBuffer>(phys_dev, dev);

	std::vector<vk::DescriptorSetLayoutBinding> bindings = {
		uniform_buffer->binding(0),
	};

	/* BSP loader */
	bsp = std::make_unique<HLBSP::BSP>(phys_dev, dev, "assets/maps/dmc.bsp");
	bsp_shaders = {
		{ dev, "assets/shaders/bin/bsp.vert.spv", vk::ShaderStageFlagBits::eVertex },
		{ dev, "assets/shaders/bin/bsp.frag.spv", vk::ShaderStageFlagBits::eFragment },
	};

	bsp->pipeline = std::make_unique<GraphicsPipeline>(dev, bsp_shaders, swapchain->extent, *render_pass, bindings, bsp->vertex_buffer->binding(0), bsp->vertex_buffer->attrs(0));
	bsp->pipeline->update(0, *uniform_buffer);

	/* bounding and hitboxs */
	box_shaders = {
		{ dev, "assets/shaders/bin/box.vert.spv", vk::ShaderStageFlagBits::eVertex },
		{ dev, "assets/shaders/bin/box.geom.spv", vk::ShaderStageFlagBits::eGeometry },
		{ dev, "assets/shaders/bin/box.frag.spv", vk::ShaderStageFlagBits::eFragment },
	};

	std::vector<BoxVertex> boxes;
	boxes.reserve(bsp->leaves.size());
	for (auto& leaf : bsp->leaves) {
		boxes.push_back(BoxVertex{
			.mins = leaf.bb_mins,
			.maxes = leaf.bb_maxes,
		});
	}
	box_buffer = std::make_unique<GeneralVertexBuffer<BoxVertex>>(phys_dev, dev, boxes.size());
	box_buffer->upload(boxes);

	box_pipeline = std::make_unique<GraphicsPipeline>(dev, box_shaders, swapchain->extent, *render_pass, bindings, box_buffer->binding(0), box_buffer->attrs(0), GraphicsPipeline::Type::eBOX);
	box_pipeline->update(0, *uniform_buffer);

	ui = std::make_unique<UI>(this);
}

std::vector<Texture> Renderer::createResources(const std::vector<std::string>& names) {
	std::vector<Texture> ret;

	CommandBuffer texture_cmd(dev, queue_family);

	texture_cmd.begin();

	for (const auto& name : names) {
		ret.push_back({phys_dev, dev, texture_cmd, name, false});
	}

	texture_cmd.end();

	auto texture_creation_fence = dev.createFence(vk::FenceCreateInfo {});
	dev.resetFences(texture_creation_fence);

	queue.submit(vk::SubmitInfo {
		.commandBufferCount = 1,
		.pCommandBuffers = texture_cmd,
	}, texture_creation_fence);

	if (dev.waitForFences(texture_creation_fence, vk::True, UINT64_MAX) != vk::Result::eSuccess) {
		Log::error("Failed to create textures\n");
	}

	dev.destroyFence(texture_creation_fence);

	for (auto& tex : ret) {
		tex.finishCreation();
	}

	texture_cmd.cleanup(dev);

	return ret;
}

void Renderer::draw() {

	if(dev.waitForFences(render_fence, true, UINT64_MAX) != vk::Result::eSuccess) {
		Log::error("Failed to wait for fences in draw()\n");
	}

	dev.resetFences(render_fence);

	/* check if the swapchain is still good (no resize) */
	auto image_ret = dev.acquireNextImageKHR(*swapchain, UINT64_MAX, image_wait_semaphore);
	if (image_ret.result == vk::Result::eErrorOutOfDateKHR) {
		swapchain->recreate();
	}

	current_image_idx = image_ret.value;

	/* prepare command buffer for recording commands */
	command_buffer->recycle();
	command_buffer->begin();
	
	vk::ClearValue clear_values[] = {
		vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f),
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

	/* flip viewport */
	auto viewport = vk::Viewport{ 
		.x = 0.0f,
		.y = static_cast<float>(swapchain->extent.height),
		.width = static_cast<float>(swapchain->extent.width),
		.height = -static_cast<float>(swapchain->extent.height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	auto scissor = vk::Rect2D {
		.offset = {0, 0},
		.extent = win.getDimensions(),
	};
	
	/* no secondary command buffers (yet), so contents are passed inline */
	command_buffer->command_buffer.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);

	auto sz = win.getDimensions();

	const auto p = glm::perspective(glm::radians(90.0f), static_cast<float>(sz.width) / static_cast<float>(sz.height), near_plane, far_plane);

	auto uni = UniformData{
		.view = cam.view(),
		.proj = p,
		.time = time,
		.cam_pos = cam.pos,
		.cam_dir = cam.dir(),
		.viewport = glm::vec2(viewport.width, viewport.y),
		.tess_factor = tess_factor,
		.tess_edge_size = tess_edge_size,
	};

	std::memcpy(uni.frustum, frustum(p * uni.view).data(), sizeof(uni.frustum));

	uniform_buffer->upload(uni);

	command_buffer->command_buffer.setViewport(0, viewport);
	command_buffer->command_buffer.setScissor(0, scissor);

	bsp->load_vertices(cam.pos, visibility_testing, p * uni.view);
	command_buffer->bind(bsp.get());
	/*command_buffer->draw(bsp->vertices.size(), 1);*/
	command_buffer->draw(bsp->textured_vertices.size(), 1);

	n_indices = bsp->textured_vertices.size();

	if (show_bboxes) {
		command_buffer->bind(*box_pipeline);
		command_buffer->bind(*box_buffer);
		command_buffer->draw(box_buffer->buffer->size / sizeof(BoxVertex), 1);
	}

	/* draw User Interface stuff */
	ui->newFrame();

	ui->render(command_buffer->command_buffer);

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
			Log::info("Recreating swapchain\n");
			swapchain->recreate();
		break;
		default:
			Log::error("Failed to present surface.\n");
		break;
	}

	frame++;
	time += frametime / 1000.0 * speed * static_cast<float>(!paused);
}

Renderer::~Renderer() {
	dev.waitIdle();

	ui.reset();
	bsp.reset();

	box_buffer.reset();
	box_pipeline.reset();
	uniform_buffer.reset();

	for (auto& shader : box_shaders)
		shader.cleanup();
	for (auto& shader : bsp_shaders)
		shader.cleanup();

	for (auto& tex : textures) {
		tex.cleanup();
	}

	swapchain.reset();

	dev.destroySemaphore(image_wait_semaphore);
	dev.destroySemaphore(render_wait_semaphore);
	dev.destroyFence(render_fence);

	command_buffer->cleanup(dev);
	render_pass->cleanup(dev);
	dev.destroy();
	instance.destroySurfaceKHR(surface);
	instance.destroy();
}
