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

	std::unique_ptr<Buffer> staging;

	Texture(vk::PhysicalDevice phys_dev, vk::Device dev, CommandBuffer& command_buffer, const std::string& fname);

	inline vk::DescriptorSetLayoutBinding binding(uint32_t binding, vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eAll) {
		return vk::DescriptorSetLayoutBinding {
			.binding = binding,
			.descriptorType = vk::DescriptorType::eCombinedImageSampler,
			.descriptorCount = 1,
			.stageFlags = vk::ShaderStageFlagBits::eFragment,
			.pImmutableSamplers = nullptr,
		};
	}

	/* seperates out steps that need to be taken *after* command buffer is submitted */
	void finishCreation();


	void cleanup();
};