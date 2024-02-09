#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Memory/Buffer.hpp>

#include <glm/glm.hpp>

#include <memory>

/* Uniform:
 * 
layout (set = 0, binding = 0) uniform Matrices {
	vec3 cam_pos;
	float time;
	vec4 viewport;
	vec3 cam_dir;
}; 
 */

struct UniformData {
	glm::vec3 cam_pos;
	float time;
	glm::vec4 viewport;
	glm::vec3 cam_dir;
};

struct UniformBuffer {
	UniformBuffer(vk::PhysicalDevice phys_dev, vk::Device dev);

	std::unique_ptr<Buffer> buffer;

	void upload(const UniformData& data);

	inline operator vk::Buffer& () const {
		return *buffer;
	}

	inline vk::DescriptorSetLayoutBinding binding(uint32_t binding, vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eAll) {
		return vk::DescriptorSetLayoutBinding {
			.binding = binding,
			.descriptorType = vk::DescriptorType::eUniformBuffer,
			.descriptorCount = 1,
			.stageFlags = stages,
			.pImmutableSamplers = nullptr,
		};
	}
};