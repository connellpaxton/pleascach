#pragma once

#include <util/glsl_types.hpp>

#include <Scene/Object.hpp>

#include <Memory/Buffer.hpp>

using namespace glsl;

/* Wrapper for SSBO */
struct ShaderBuffer {

	ShaderBuffer(vk::PhysicalDevice phys_dev, vk::Device dev, const size_t n_objects = 0x1000) : phys_dev(phys_dev), dev(dev), n_objects(n_objects) {
		buffer = std::make_unique<Buffer>(
			phys_dev, dev, n_objects * sizeof(Object),
			vk::BufferUsageFlagBits::eStorageBuffer,
			vk::MemoryPropertyFlagBits::eHostCoherent
			| vk::MemoryPropertyFlagBits::eHostVisible
		);
		objects = reinterpret_cast<Object*>(buffer->p);
	}

	vk::PhysicalDevice phys_dev;
	vk::Device dev;
	size_t n_objects;
	Object* objects;

	std::unique_ptr<Buffer> buffer;

	operator vk::Buffer&() const {
		return *buffer;
	}

	inline vk::DescriptorSetLayoutBinding binding(uint32_t binding, vk::ShaderStageFlags stages = vk::ShaderStageFlagBits::eFragment) {
		return vk::DescriptorSetLayoutBinding {
			.binding = binding,
			.descriptorType = vk::DescriptorType::eStorageBuffer,
			.descriptorCount = 1,
			.stageFlags = stages,
			.pImmutableSamplers = nullptr,
		};
	}

	inline void upload(const std::vector<Object>& scene) {
		buffer->upload(reinterpret_cast<const uint8_t*>(scene.data()));
	}

	~ShaderBuffer() {
		buffer.reset();
	}
};