#pragma once


#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Renderer/Renderer.hpp>

struct RenderPass {
	RenderPass(vk::Device dev, Swapchain* swp);
	~RenderPass();

	vk::Device dev;
	vk::RenderPass render_pass;

	operator vk::RenderPass& () {
		return render_pass;
	}

};