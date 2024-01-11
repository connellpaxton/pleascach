#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Renderer;
struct Swapchain;
struct RenderPass;


struct Framebuffer {
	std::vector<vk::Framebuffer> framebuffers;
	std::vector<VkImage> images;
	std::vector<VkImageView> image_views;
	vk::Device dev;
	Swapchain& swp;

	Framebuffer(vk::Device dev, const RenderPass* render_pass, Swapchain& swp);
	~Framebuffer();
};