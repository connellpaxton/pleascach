#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Memory/Image.hpp>
#include <memory>

struct CommandBuffer;
struct Buffer;

struct Texture {
	vk::Device dev;
	std::unique_ptr<Image> image;
	vk::ImageView view;
	vk::Sampler sampler;
	bool free_memory;
	uint8_t* image_data;
	vk::Extent3D extent;

	std::unique_ptr<Buffer> staging;

	Texture(vk::PhysicalDevice phys_dev, vk::Device dev, CommandBuffer& command_buffer, const std::string& fname, bool free_memory = true);

	inline vk::DescriptorSetLayoutBinding binding(uint32_t binding, vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eAll) {
		return vk::DescriptorSetLayoutBinding {
			.binding = binding,
			.descriptorType = vk::DescriptorType::eCombinedImageSampler,
			.descriptorCount = 1,
			.stageFlags = vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eTessellationControl | vk::ShaderStageFlagBits::eTessellationEvaluation,
			.pImmutableSamplers = nullptr,
		};
	}

	/* seperates out steps that need to be taken *after* command buffer is submitted */
	void finishCreation();


	void cleanup();
};