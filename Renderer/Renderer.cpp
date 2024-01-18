#include <renderer/renderer.hpp>
#include <window/window.hpp>

#include <util/log.hpp>

using namespace std::string_literals;

Renderer::Renderer(Window& win) : win(win) {
	/* Create Instance object */
	auto app_info = vk::ApplicationInfo {
		.pApplicationName = "Pléascach Demo",
		.applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
		.pEngineName = "Pléascach",
		.engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
		.apiVersion = VK_API_VERSION_1_0,
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
#ifdef _DEBUG

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
		.enabledExtensionCount = static_cast<u32>(extensions.size()),
		.ppEnabledExtensionNames = extension_names.data(),
	};

#else
	auto inst_info = vk::InstanceCreateInfo{
		.pApplicationInfo = &app_info,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = nullptr,
		.enabledExtensionCount = static_cast<u32>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data(),
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
	auto dev_extentions = phys_dev.enumerateDeviceExtensionProperties();
	Log::info("%zu available device extensions\n", dev_extentions.size());
	for (const auto& ext : dev_extentions) {
		Log::info("\t\"%s\"\n", ext.extensionName.data());
		required_extensions.push_back(ext.extensionName);
	}
	
	auto dev_layers = phys_dev.enumerateDeviceLayerProperties();
	Log::info("%zu available device layers\n", dev_layers.size());
	for (const auto& layer : dev_layers) {
		Log::info("\t\"%s\"\n", layer.layerName.data());
	}
	std::vector<const char*> dev_layer_names = {
		"VK_LAYER_KHRONOS_validation"
	};

#pragma message("TODO: MAKE THIS NO LONGER EVERY SINGLE EXTENTION NAME")
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

	swapchain = std::make_unique<Swapchain>(dev, surface, win.getDimensions());

	queue = dev.getQueue(queue_family, 0);
}

void Renderer::draw() {
	/* check if the swapchain is still good (no resize) */
	auto image_ret = dev.acquireNextImageKHR(*swapchain, UINT64_MAX);
	if (image_ret.result == vk::Result::eErrorOutOfDateKHR) {
		swapchain->recreate(win.getDimensions());
	}

	current_image_idx = image_ret.value;

	command_buffer->recycle();
	command_buffer->begin();

	command_buffer->end();
}

void Renderer::present() {
	auto present_info = vk::PresentInfoKHR{
		.swapchainCount = 1,
		.pSwapchains = &swapchain->operator vk::SwapchainKHR &(),
		.pImageIndices = &current_image_idx,
	};

	switch (queue.presentKHR(present_info)) {
		case vk::Result::eSuccess:
		break;
		case vk::Result::eSuboptimalKHR:
		case vk::Result::eErrorOutOfDateKHR:
			swapchain->recreate(win.getDimensions());
		break;
		default:
			Log::error("Failed to present surface.");
		break;
	}
}

Renderer::~Renderer() {
	command_buffer->cleanup(dev);
	swapchain.reset();
	dev.waitIdle();
	dev.destroy();
	instance.destroySurfaceKHR(surface);
	instance.destroy();
}
