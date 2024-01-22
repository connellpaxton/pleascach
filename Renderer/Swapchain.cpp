#include <Renderer/Swapchain.hpp>

Swapchain::Swapchain(vk::Device& dev, const vk::SurfaceKHR& surface, const vk::Extent2D& extent, RenderPass render_pass, vk::ImageView depth_image_view)
		: dev(dev), surface(surface), render_pass(render_pass), depth_image_view(depth_image_view) {
	create(extent);
}

void Swapchain::create(const vk::Extent2D& extent, vk::SwapchainKHR old_swapchain) {
	this->extent = extent;

	auto format = vk::Format::eB8G8R8A8Unorm;

	auto swap_info = vk::SwapchainCreateInfoKHR{
		.surface = surface,
		/* at least double-buffered */
		.minImageCount = 3,
		.imageFormat = format,
		.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
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
		.presentMode = vk::PresentModeKHR::eFifo,
		.clipped = VK_TRUE,
		.oldSwapchain = old_swapchain,
	};

	swapchain = dev.createSwapchainKHR(swap_info);

	images = dev.getSwapchainImagesKHR(swapchain);
	views.resize(images.size());
	framebuffers.resize(images.size());
	for (size_t i = 0; i < views.size(); i++) {
		auto color_image_info = vk::ImageViewCreateInfo{
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


void Swapchain::recreate(const vk::Extent2D& extent) {
	dev.waitIdle();
	cleanup();
	create(extent, swapchain);
}

void Swapchain::cleanup() {
	for(auto& fb : framebuffers)
		dev.destroyFramebuffer(fb);
	dev.destroySwapchainKHR(swapchain);
}

Swapchain::~Swapchain() {
	cleanup();
}
