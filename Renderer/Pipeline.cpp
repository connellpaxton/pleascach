#include <Renderer/Pipeline.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/RenderPass.hpp>
#include <Renderer/UniformBuffer.hpp>
#include <Renderer/VertexBuffer.hpp>

#include <map>

#include <util/log.hpp>

GraphicsPipeline::GraphicsPipeline(vk::Device dev, const std::vector<Shader>& shaders, const vk::Extent2D& extent, const RenderPass& render_pass, vk::ArrayProxy<vk::DescriptorSetLayoutBinding> bindings, const VertexBuffer& vertex_buffer) : dev(dev) {
	/* create layout
	 * Eventually add a graphicspipline constructor that allows specification of layouts etc
	 * kinda like how Image::Image has all those versions
	 */
	 /* descriptor set layouts simply define a group of resources that can be relied upon */
	desc_layout = dev.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo {
		.bindingCount = bindings.size(),
		.pBindings = bindings.data(),
	});

	layout = dev.createPipelineLayout(vk::PipelineLayoutCreateInfo {
		.setLayoutCount = 1,
		.pSetLayouts = &desc_layout,
	});

	/* track how much of each type to allocate */
	std::map<vk::DescriptorType, uint32_t> types;

	for (const auto& binding : bindings) {
		types[binding.descriptorType]++;
	}

	std::vector<vk::DescriptorPoolSize> sizes;
	sizes.reserve(types.size());

	for (const auto& p : types) {
		sizes.push_back(vk::DescriptorPoolSize {
			.type = p.first,
			.descriptorCount = p.second,
		});
	}

	desc_pool = dev.createDescriptorPool(vk::DescriptorPoolCreateInfo {
		.maxSets = 1,
		.poolSizeCount = static_cast<uint32_t>(sizes.size()),
		.pPoolSizes = sizes.data(),
	});

	desc_set = dev.allocateDescriptorSets(vk::DescriptorSetAllocateInfo {
		.descriptorPool = desc_pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &desc_layout,
	})[0];

	/* shader setup */
	std::vector<vk::PipelineShaderStageCreateInfo> shader_info;
	shader_info.reserve(shaders.size());
	for (const auto& shader : shaders)
		shader_info.push_back(shader.info());

	/* vertex input setup */
	const std::vector<vk::VertexInputBindingDescription> vertex_bindings = {
		vertex_buffer.binding(0),
	};

	auto attrs = vertex_buffer.attrs(0);

	const auto vertex_input_info = vk::PipelineVertexInputStateCreateInfo{
		.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_bindings.size()),
		.pVertexBindingDescriptions = vertex_bindings.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrs.size()),
		.pVertexAttributeDescriptions = attrs.data(),
	};

	const auto input_asm_info = vk::PipelineInputAssemblyStateCreateInfo {
		.topology = vk::PrimitiveTopology::eTriangleList,
		/* matters later if we use strip primitives */
		.primitiveRestartEnable = vk::False,
	};

	const auto raster_info = vk::PipelineRasterizationStateCreateInfo {
		.depthClampEnable = vk::False,
		.polygonMode = vk::PolygonMode::eFill,
		.cullMode = vk::CullModeFlagBits::eBack,
		.frontFace = vk::FrontFace::eClockwise,
		.depthBiasEnable = vk::False,
		.lineWidth = 1.0,
	};

	const auto multisample_info = vk::PipelineMultisampleStateCreateInfo {
		.rasterizationSamples = vk::SampleCountFlagBits::e1,
		.sampleShadingEnable = vk::False,
	};

	const auto depth_stencil_info = vk::PipelineDepthStencilStateCreateInfo{
		.depthTestEnable = vk::True,
		.depthWriteEnable = vk::True,
		.depthCompareOp = vk::CompareOp::eLessOrEqual,
		.depthBoundsTestEnable = vk::False,
		.stencilTestEnable = vk::False,
		.minDepthBounds = 0.0,
		.maxDepthBounds = 1.0,
	};
	
	const auto color_blend_attachment = vk::PipelineColorBlendAttachmentState{
		.blendEnable = vk::False,
		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
						| vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eA,
	};

	const std::array<float, 4> blend_constants = {0.0f, 0.0f, 0.0f, 0.0f};
	const auto color_blend_info = vk::PipelineColorBlendStateCreateInfo{
		.logicOpEnable = vk::False,
		.logicOp = vk::LogicOp::eCopy,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment,
		.blendConstants = blend_constants,
	};

	/* temporary viewport and scissor, since it is a dynamic state due to the existence of window resizing */
	const auto viewport = vk::Viewport{
		.x = 0.0,
		.y = 0.0,
		.width = static_cast<float>(extent.width),
		.height = static_cast<float>(extent.height),
	};

	const auto scissor = vk::Rect2D {
		.offset = 0,
		.extent = extent,
	};

	const auto viewport_info = vk::PipelineViewportStateCreateInfo {
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor,
	};

	const vk::DynamicState dyn_states[] = {
		vk::DynamicState::eScissor,
		vk::DynamicState::eViewport,
	};

	const auto dyn_info = vk::PipelineDynamicStateCreateInfo{
		.dynamicStateCount = std::size(dyn_states),
		.pDynamicStates = dyn_states,
	};

	const auto pipeline_info = vk::GraphicsPipelineCreateInfo {
		.stageCount = static_cast<uint32_t>(shaders.size()),
		.pStages = shader_info.data(),
		.pVertexInputState = &vertex_input_info,
		.pInputAssemblyState = &input_asm_info,
		.pTessellationState = nullptr,
		.pViewportState = &viewport_info,
		.pRasterizationState = &raster_info,
		.pMultisampleState = &multisample_info,
		.pDepthStencilState = &depth_stencil_info,
		.pColorBlendState = &color_blend_info,
		.pDynamicState = &dyn_info,
		.layout = layout,
		.renderPass = render_pass,
		.basePipelineHandle = nullptr,
	};

	auto res = dev.createGraphicsPipeline(nullptr, pipeline_info);
	if (res.result != vk::Result::eSuccess) {
		Log::error("Failed to create pipeline: (Vulkan error code: %d)\n", res.result);
	}

	pipeline = res.value;
}

void GraphicsPipeline::update(uint32_t binding, const UniformBuffer& uni) {
	auto buff_info = vk::DescriptorBufferInfo {
		.buffer = uni,
		.offset = 0,
		.range = vk::WholeSize,
	};
	dev.updateDescriptorSets(vk::WriteDescriptorSet {
		.dstSet = desc_set,
		.dstBinding = binding,
		.descriptorCount = 1,
		.descriptorType = vk::DescriptorType::eUniformBuffer,
		.pBufferInfo = &buff_info,
	}, nullptr);
}

GraphicsPipeline::~GraphicsPipeline() {
	dev.destroyDescriptorSetLayout(desc_layout);
	dev.destroyPipelineLayout(layout);
	dev.destroyDescriptorPool(desc_pool);
	dev.destroyPipeline(pipeline);
}