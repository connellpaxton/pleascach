#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Shader {
	vk::Device dev;
	vk::ShaderModule module;
	vk::ShaderStageFlagBits stage;

	Shader(vk::Device dev, const std::string& fname, vk::ShaderStageFlagBits stage);

	inline operator vk::ShaderModule() const {
		return module;
	}

	inline operator vk::ShaderModule& () {
		return module;
	}

	inline const vk::PipelineShaderStageCreateInfo info() const {
		return vk::PipelineShaderStageCreateInfo {
			.stage = stage,
			.module = module,
			.pName = "main",
		};
	}

	std::string fname;

	void cleanup();
};
