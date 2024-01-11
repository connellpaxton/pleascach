#include <Renderer/Shader.hpp>

#include <iostream>
#include <fstream>

Shader::Shader(const std::string& filename, vk::Device& dev, const std::string& entry) : dev(dev) {
	auto f = std::ifstream(filename);
	std::string	src((std::istreambuf_iterator<char>(f)),
				(std::istreambuf_iterator<char>()));

	vk::ShaderModuleCreateInfo shader_info {
		.codeSize = src.length(),
		.pCode = reinterpret_cast<const uint32_t*>(src.c_str()),
	};

	shader = dev.createShaderModule(shader_info);
	std::cerr << "Successfully loaded shader: " << filename << std::endl;

	/* deduce type from filename */
	vk::ShaderStageFlagBits stage = vk::ShaderStageFlagBits::eAllGraphics;

	if (filename.find(".frag") != std::string::npos) {
		stage = vk::ShaderStageFlagBits::eFragment;
	} else if (filename.find(".vert") != std::string::npos) {
		stage = vk::ShaderStageFlagBits::eVertex;
	}

	stage_info = vk::PipelineShaderStageCreateInfo{
		.stage = stage,
		.module = shader,
		.pName = entry.c_str()
	};
}

Shader::~Shader() {
	dev.destroyShaderModule();
}