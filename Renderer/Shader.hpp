#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Shader {
	vk::ShaderModule module;

	Shader(vk::Device dev, const std::string& fname);
	void cleanup(vk::Device dev);



	inline operator vk::ShaderModule& () {
		return module;
	}

	std::string fname;

};