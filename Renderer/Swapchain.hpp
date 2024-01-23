#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>


#include <Renderer/RenderPass.hpp>

struct Window;
struct Image;

struct Swapchain {
	Swapchain(Window& win, vk::Device dev, vk::PhysicalDevice phys_dev, const vk::SurfaceKHR& surface, RenderPass render_pass);
	vk::SwapchainKHR swapchain;
	inline operator vk::SwapchainKHR& () {
		return swapchain;
	}

	struct Capabilities {
		vk::SurfaceCapabilitiesKHR surface_caps;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;
		Capabilities(vk::PhysicalDevice phys_dev, vk::SurfaceKHR surface);

		vk::Extent2D chooseExtent(vk::Extent2D extent);
		vk::SurfaceFormatKHR chooseFormat();
	};


	Window& win;
	vk::Device dev;
	vk::PhysicalDevice phys_dev;
	vk::SurfaceKHR surface;
	std::vector<vk::Image> images;
	std::vector<vk::ImageView> views;
	std::vector<vk::Framebuffer> framebuffers;

	std::unique_ptr<Image> depth_image;
	vk::ImageView depth_image_view;

	vk::Format format;

	RenderPass render_pass;

	vk::Extent2D extent;

	void create(vk::SwapchainKHR old_swapchain = nullptr);
	void recreate();
	void cleanup();
	~Swapchain();
};