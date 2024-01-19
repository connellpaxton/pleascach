#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Shader;

struct ComputePipeline {
	ComputePipeline(vk::Device dev, const Shader& shader);

	vk::Pipeline pipeline;
	inline operator vk::Pipeline&() {
		return pipeline;
	}
};


struct GraphicsPipeline {
	GraphicsPipeline(vk::Device dev);
	vk::Pipeline pipeline;
	
	inline operator vk::Pipeline&() {
		return pipeline;
	}
};


