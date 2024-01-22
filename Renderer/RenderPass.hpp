#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct RenderPass {
	vk::RenderPass render_pass;

	RenderPass(vk::Device dev, vk::Format image_format, vk::Format depth_format = vk::Format::eD16Unorm);

	void cleanup(vk::Device dev);

	operator vk::RenderPass&() {
		return render_pass;
	}
};