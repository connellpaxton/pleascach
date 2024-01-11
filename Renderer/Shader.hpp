#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <string>

struct Shader {
	Shader(const std::string& filename, vk::Device& dev, const std::string& entry = "main");
	~Shader();

	vk::PipelineShaderStageCreateInfo stage_info;
	vk::ShaderModule shader;
	vk::Device& dev;

	operator vk::ShaderModule& () {
		return shader;
	}
};
