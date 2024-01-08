#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include <vkb/VkBootstrap.h>

#include <Renderer/Framebuffer.hpp>

#include <memory>

struct Renderer;
struct Window;

struct Swapchain {
	Renderer* ren;

	VmaAllocation depth_image_alloc;

	vkb::Swapchain swapchain;
	vk::Image depth_image;
	vk::ImageView depth_image_view;
	std::unique_ptr<Framebuffer> framebuffer;

	Swapchain(Renderer* ren);
	void create();
	void recreate();
	void cleanup();
	~Swapchain();

	inline operator vkb::Swapchain () {
		return swapchain;
	}
};