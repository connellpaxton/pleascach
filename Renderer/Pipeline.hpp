#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

struct Shader;
struct UniformBuffer;
struct VertexBuffer;
struct RenderPass;
struct Texture;

struct GraphicsPipeline {
	GraphicsPipeline(vk::Device dev, const std::vector<Shader>& shaders,
				const vk::Extent2D& extent, const RenderPass& render_pass,
				vk::ArrayProxy<vk::DescriptorSetLayoutBinding> bindings,
				const VertexBuffer& vertex_buffer);
	vk::Device dev;
	vk::Pipeline pipeline;
	vk::PipelineLayout layout;
	vk::DescriptorSetLayout desc_layout;
	vk::DescriptorPool desc_pool;
	vk::DescriptorSet desc_set;

	inline operator vk::Pipeline&() {
		return pipeline;
	}

	/* create overload for every type of object we need to update */
	void update(uint32_t binding, const UniformBuffer& uni);
	void update(uint32_t binding, const Texture& tex);

	~GraphicsPipeline();
};


