#include <renderer/swapchain.hpp>

Swapchain::Swapchain(vk::Device& dev, const vk::SurfaceKHR& surface, const vk::Extent2D& extent) : dev(dev), surface(surface) {
	create(extent);
}

void Swapchain::create(const vk::Extent2D& extent, vk::SwapchainKHR old_swapchain) {

	auto swap_info = vk::SwapchainCreateInfoKHR{
		.surface = surface,
		/* at least double-buffered */
		.minImageCount = 3,
		.imageFormat = vk::Format::eR8G8B8A8Unorm,
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
}


void Swapchain::recreate(const vk::Extent2D& extent) {
	dev.waitIdle();
	cleanup();
	create(extent);
}

void Swapchain::cleanup() {
	dev.destroySwapchainKHR(swapchain);
}

Swapchain::~Swapchain() {
	cleanup();
}