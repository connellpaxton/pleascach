#include <Renderer/Swapchain.hpp>
#include <Window/Window.hpp>

#include <Memory/Memory.hpp>
#include <Memory/Image.hpp>

#include <util/log.hpp>

/* TODO: Make solution that doesn't involve using GLFW directly here for minimization */
#include <GLFW/glfw3.h>


Swapchain::Swapchain(Window& win, vk::Device dev, vk::PhysicalDevice phys_dev, const vk::SurfaceKHR& surface, RenderPass render_pass)
		: win(win), dev(dev), phys_dev(phys_dev), surface(surface), render_pass(render_pass) {
	create();
}

Swapchain::Capabilities::Capabilities(vk::PhysicalDevice phys_dev, vk::SurfaceKHR surface) {
	surface_caps = phys_dev.getSurfaceCapabilitiesKHR(surface);
	formats = phys_dev.getSurfaceFormatsKHR(surface);
	present_modes = phys_dev.getSurfacePresentModesKHR(surface);
}

vk::Extent2D Swapchain::Capabilities::chooseExtent(vk::Extent2D extent) {
	if (surface_caps.currentExtent.width != UINT32_MAX)
		return surface_caps.currentExtent;

	extent.width = std::clamp(extent.width, surface_caps.minImageExtent.width, surface_caps.maxImageExtent.width);
	extent.height = std::clamp(extent.height, surface_caps.minImageExtent.height, surface_caps.maxImageExtent.height);

	return extent;
}

vk::SurfaceFormatKHR Swapchain::Capabilities::chooseFormat() {
	for (const auto& format : formats) {
		if (format.format == vk::Format::eB8G8R8A8Snorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			return format;
	}
	return formats[0];
}

void Swapchain::create(vk::SwapchainKHR old_swapchain) {
	Capabilities caps(phys_dev, surface);
	extent = caps.chooseExtent(win.getDimensions());
	auto sFormat = caps.chooseFormat();
	format = sFormat.format;

	auto swap_info = vk::SwapchainCreateInfoKHR {
		.surface = surface,
		.minImageCount = caps.surface_caps.minImageCount,
		.imageFormat = format,
		.imageColorSpace = sFormat.colorSpace,
		.imageExtent = extent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = nullptr,
		.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,
		/* see if this allows see through windows on Wayland */
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		/* waits for refresh (V-Sync), consider playing with relaxed fifo later on*/
//		.presentMode = vk::PresentModeKHR::eFifoRelaxed,
		.presentMode = vk::PresentModeKHR::eMailbox,
		.clipped = VK_TRUE,
		.oldSwapchain = old_swapchain,
	};

	Log::info("Swapchain recreated with %d minImageCount\n", swap_info.minImageCount);

	swapchain = dev.createSwapchainKHR(swap_info);

	images = dev.getSwapchainImagesKHR(swapchain);

	depth_image = std::make_unique<Image>(phys_dev, dev, vk::Extent3D { extent.width, extent.height, 1 }, vk::Format::eD16Unorm,
		vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal);

	auto depth_view_info = vk::ImageViewCreateInfo {
		.image = depth_image->image,
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

	views.resize(images.size());
	framebuffers.resize(images.size());
	for (size_t i = 0; i < views.size(); i++) {
		auto color_image_info = vk::ImageViewCreateInfo {
			.image = images[i],
			.viewType = vk::ImageViewType::e2D,
			.format = format,
			.components = {
				.r = vk::ComponentSwizzle::eR,
				.g = vk::ComponentSwizzle::eG,
				.b = vk::ComponentSwizzle::eB,
				.a = vk::ComponentSwizzle::eA,
			},
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};
		views[i] = dev.createImageView(color_image_info);

		vk::ImageView attachments[] = { views[i], depth_image_view };
		auto framebuffer_info = vk::FramebufferCreateInfo {
			.renderPass = render_pass,
			.attachmentCount = static_cast<uint32_t>(std::size(attachments)),
			.pAttachments = attachments,
			.width = extent.width,
			.height = extent.height,
			.layers = 1,
		};
		
		framebuffers[i] = dev.createFramebuffer(framebuffer_info);
	}
}


void Swapchain::recreate() {
	vk::Extent2D ext;
	do {
		ext = win.getDimensions();
		glfwWaitEvents();
	} while (!ext.width || !ext.height);

	dev.waitIdle();
	cleanup();
	auto save = swapchain;
	create(swapchain);
	dev.destroySwapchainKHR(save);
}

void Swapchain::cleanup() {
	dev.waitIdle();
	for(auto& fb : framebuffers)
		dev.destroyFramebuffer(fb);
	for (auto& view : views)
		dev.destroyImageView(view);

	dev.destroyImageView(depth_image_view);
	depth_image->cleanup();
}

Swapchain::~Swapchain() {
	cleanup();
	dev.destroySwapchainKHR(swapchain);
}

