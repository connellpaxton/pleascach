#include <Renderer/Swapchain.hpp>
#include <Window/Window.hpp>

#include <Memory/Memory.hpp>

#include <util/log.hpp>

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
		.minImageCount = caps.surface_caps.maxImageCount,
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
		.presentMode = vk::PresentModeKHR::eFifo,
		.clipped = VK_TRUE,
		.oldSwapchain = old_swapchain,
	};

	swapchain = dev.createSwapchainKHR(swap_info);

	images = dev.getSwapchainImagesKHR(swapchain);

	auto depth_image_info = vk::ImageCreateInfo{
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

	auto depth_alloc_info = vk::MemoryAllocateInfo{
		.allocationSize = depth_mem_reqs.size,
		.memoryTypeIndex = mem::choose_heap(phys_dev, depth_mem_reqs, vk::MemoryPropertyFlagBits::eDeviceLocal),
	};

	depth_alloc = dev.allocateMemory(depth_alloc_info);
	dev.bindImageMemory(depth_image, depth_alloc, 0);

	auto depth_view_info = vk::ImageViewCreateInfo{
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
	dev.destroyImage(depth_image);
	dev.freeMemory(depth_alloc);
}

Swapchain::~Swapchain() {
	cleanup();
	dev.destroySwapchainKHR(swapchain);
}

