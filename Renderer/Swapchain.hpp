#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Swapchain {
	Swapchain(vk::Device& dev, const vk::SurfaceKHR& surface, const vk::Extent2D& extent);
	vk::SwapchainKHR swapchain;
	inline operator vk::SwapchainKHR& () {
		return swapchain;
	}

	vk::Device& dev;
	vk::SurfaceKHR surface;
	std::vector<vk::Image> images;
	std::vector<vk::ImageView> views;

	void create(const vk::Extent2D& extent, vk::SwapchainKHR old_swapchain = nullptr);
	void recreate(const vk::Extent2D& extent);
	void cleanup();
	~Swapchain();
};