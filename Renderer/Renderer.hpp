#pragma once

#include <memory>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <renderer/swapchain.hpp>
#include <renderer/command_buffer.hpp>

struct Window;

/* Contains all of the Vulkan objects involved in rendering the scene */
struct Renderer {
	Renderer(Window& win);
	~Renderer();

	void draw();
	void present();

	Window& win;
	vk::Instance instance;
	vk::Device dev;
	vk::SurfaceKHR surface;
	std::unique_ptr<Swapchain> swapchain;
	int queue_family;
	vk::Queue queue;
	std::unique_ptr<CommandBuffer> command_buffer;
	uint32_t current_image_idx;
};