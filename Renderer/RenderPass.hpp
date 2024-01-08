#pragma once


#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Renderer/Renderer.hpp>

struct RenderPass {
	RenderPass(Renderer* ren, Swapchain* swp);
	~RenderPass();

	Renderer* ren;
	vk::RenderPass render_pass;
};