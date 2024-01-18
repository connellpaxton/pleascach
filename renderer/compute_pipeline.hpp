#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct ComputePipeline {
	ComputePipeline(vk::Device dev) {
	}

	vk::Pipeline pipeline;
	inline operator vk::Pipeline&() {
		return pipeline;
	}
};

