#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>


#include <Renderer/RenderPass.hpp>


struct Swapchain {
	Swapchain(vk::Device& dev, const vk::SurfaceKHR& surface, const vk::Extent2D& extent, RenderPass render_pass, vk::ImageView depth_image_view);
	vk::SwapchainKHR swapchain;
	inline operator vk::SwapchainKHR& () {
		return swapchain;
	}

	vk::Device& dev;
	vk::SurfaceKHR surface;
	std::vector<vk::Image> images;
	std::vector<vk::ImageView> views;
	std::vector<vk::Framebuffer> framebuffers;

	RenderPass render_pass;
	vk::ImageView depth_image_view;

	vk::Extent2D extent;

	void create(const vk::Extent2D& extent, vk::SwapchainKHR old_swapchain = nullptr);
	void recreate(const vk::Extent2D& extent);
	void cleanup();
	~Swapchain();
};