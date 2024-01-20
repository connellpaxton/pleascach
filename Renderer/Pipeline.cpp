#include <Renderer/Pipeline.hpp>

#include <Renderer/Shader.hpp>

ComputePipeline::ComputePipeline(vk::Device dev, const Shader& shader) {
	auto shader_info = vk::PipelineShaderStageCreateInfo {
		.stage = vk::ShaderStageFlagBits::eCompute,
		.module = shader,
		.pName = "main",
	};

	auto layout = vk::PipelineLayoutCreateInfo {

	};

	auto create_info = vk::ComputePipelineCreateInfo {
		.stage = shader_info,
	};
}