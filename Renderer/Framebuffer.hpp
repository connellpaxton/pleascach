#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Renderer;
struct Swapchain;


struct Framebuffer {
	std::vector<vk::Framebuffer> framebuffers;
	std::vector<VkImage> images;
	std::vector<VkImageView> image_views;
	Renderer* ren;

	Framebuffer(Renderer* ren, Swapchain* swp);
	~Framebuffer();
};