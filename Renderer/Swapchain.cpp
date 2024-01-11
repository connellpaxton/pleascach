#include <Renderer/Swapchain.hpp>
#include <Window/Window.hpp>

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <iostream>


Swapchain::Swapchain(Renderer* ren) : ren(ren) {
	create();
	ren->render_pass = std::make_unique<RenderPass>(ren->dev.operator VkDevice(), this);
	framebuffer = std::make_unique<Framebuffer>(ren->dev.operator VkDevice(), ren->render_pass.get(), *this);
}

void Swapchain::create() {
	/* three parts:
	1. Build Swapchain
	2. Create Depth Buffer
	3. Create Framebuffer */

	/* Part 1 */
	vkb::SwapchainBuilder swap_builder { ren->dev };

	auto swap_ret = swap_builder
		.set_old_swapchain(swapchain)
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR).build();

	if (!swap_ret) {
		throw "Failed to build swapchain"s;
	}
	swapchain = swap_ret.value();

	/* Part 2 */
	vk::Extent3D depth_extent {
		.width = ren->win->width,
			.height = ren->win->height,
			.depth = 1,
	};

	vk::ImageCreateInfo depth_image_info {
		.imageType = vk::ImageType::e2D,
			.format = vk::Format::eD32Sfloat,
			.extent = depth_extent,
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = vk::SampleCountFlagBits::e1,
			.tiling = vk::ImageTiling::eOptimal,
			.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment
	};

	VmaAllocationCreateInfo depth_alloc_info{
		.usage = VMA_MEMORY_USAGE_GPU_ONLY,
		.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
	};

	VkImage tmp_image;
	if (vmaCreateImage(ren->allocator, &depth_image_info.operator const VkImageCreateInfo & (), &depth_alloc_info, &tmp_image, &depth_image_alloc, NULL) != VK_SUCCESS) {
		throw "Failed to create image"s;
	}
	depth_image = tmp_image;

	vk::ImageViewCreateInfo depth_image_view_info {
		.image = depth_image,
			.viewType = vk::ImageViewType::e2D,
			.format = vk::Format::eD32Sfloat,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eDepth,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
		}
	};

	depth_image_view = vk::Device(ren->dev).createImageView(depth_image_view_info);
}

void Swapchain::recreate() {
	/* minization */
	ren->win->wait_minimize();
	vk::Device(ren->dev).waitIdle();

	cleanup();
	create();

	framebuffer = std::make_unique<Framebuffer>(ren->dev.operator VkDevice(), ren->render_pass.get(), *this);
}

void Swapchain::cleanup() {
	framebuffer.reset();
	vk::Device(ren->dev).destroyImageView(depth_image_view);
	vmaDestroyImage(ren->allocator, depth_image, depth_image_alloc);
}

Swapchain::~Swapchain() {
	cleanup();
}