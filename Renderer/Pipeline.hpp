#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <util/log.hpp>

#include <Renderer/VertexBuffer.hpp>

struct Shader;
struct UniformBuffer;
struct RenderPass;
struct Texture;

struct GraphicsPipeline {
	enum Type {
		eVERTEX,
		eGLTF,
		eBSP,
		eTERRAIN,
		eBOX,
	};

	GraphicsPipeline(vk::Device dev, const std::vector<Shader>& shaders,
				const vk::Extent2D& extent, const RenderPass& render_pass,
				vk::ArrayProxy<vk::DescriptorSetLayoutBinding> bindings,
				const vk::VertexInputBindingDescription& vertex_binding,
				const std::vector<vk::VertexInputAttributeDescription>& vertex_attrs,
				enum Type type = Type::eVERTEX, bool wireframe = false, bool culling = true);
	
	/* everything needed for recreation */
	vk::Device dev;
	const std::vector<Shader> shaders;
	const vk::Extent2D extent;
	const RenderPass& render_pass;
	const vk::ArrayProxy<vk::DescriptorSetLayoutBinding> bindings;
	const vk::VertexInputBindingDescription vertex_binding;
	const std::vector<vk::VertexInputAttributeDescription> vertex_attrs;
	const Type type;


	vk::Pipeline pipeline;
	vk::PipelineLayout layout;
	vk::DescriptorSetLayout desc_layout;
	vk::DescriptorPool desc_pool;
	vk::DescriptorSet desc_set;

	/* pipeline creation info cached for rebuilding */
	std::vector<vk::PipelineShaderStageCreateInfo> shader_info;
	std::vector<vk::VertexInputBindingDescription> vertex_bindings;
	vk::PipelineVertexInputStateCreateInfo vertex_input_info;
	vk::PipelineInputAssemblyStateCreateInfo input_asm_info;
	vk::PipelineTessellationStateCreateInfo tess_info;
	vk::PipelineRasterizationStateCreateInfo raster_info;
	vk::PipelineMultisampleStateCreateInfo multisample_info;
	vk::PipelineDepthStencilStateCreateInfo depth_stencil_info;
	vk::PipelineColorBlendAttachmentState color_blend_attachment;
	std::array<float, 4> blend_constants;
	vk::PipelineColorBlendStateCreateInfo color_blend_info;
	vk::Viewport viewport;
	vk::Rect2D scissor;
	vk::PipelineViewportStateCreateInfo viewport_info;
	vk::DynamicState dyn_states[2];
	vk::PipelineDynamicStateCreateInfo dyn_info;
	vk::GraphicsPipelineCreateInfo pipeline_info;


	std::vector<vk::Pipeline> defunct_pipelines;
	
	inline operator vk::Pipeline&() {
		return pipeline;
	}

	/* create overload for every type of object we need to update */
	void update(uint32_t binding, const UniformBuffer& uni);
	void update(uint32_t binding, const Texture& tex);


	void rebuild(bool wireframe, bool culling);

	~GraphicsPipeline();
};

