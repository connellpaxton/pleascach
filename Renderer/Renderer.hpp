#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>
#include <vkb/VkBootstrap.h>
#include <vma/vk_mem_alloc.h>
#include <Renderer/Swapchain.hpp>
#include <Renderer/RenderPass.hpp>
#include <Renderer/CommandBuffer.hpp>

#include <memory>

struct RenderPass;
struct Window;

struct Renderer {
	vkb::Instance instance;
	Window* win;
	VkSurfaceKHR surface;

	vkb::PhysicalDevice phys_dev;
	vkb::Device dev;
	vk::Queue queue;
	std::unique_ptr<RenderPass> render_pass;
	std::unique_ptr<Swapchain> swapchain;
	std::unique_ptr<CommandBuffer> command_buffer;

	VmaAllocator allocator;

	Renderer(Window* win);
	~Renderer();
};
