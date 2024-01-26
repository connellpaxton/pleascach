#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Shader {
	vk::Device dev;
	std::string fname;
	vk::ShaderStageFlagBits stage;
	vk::ShaderModule module;

	Shader(vk::Device dev, const std::string& fname, vk::ShaderStageFlagBits stage);
	void cleanup();

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

};
