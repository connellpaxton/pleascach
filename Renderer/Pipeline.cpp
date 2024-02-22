#include <Renderer/Pipeline.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/RenderPass.hpp>
#include <Renderer/UniformBuffer.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Resources/Texture.hpp>

#include <map>

#include <util/log.hpp>


GraphicsPipeline::GraphicsPipeline(vk::Device dev, const std::vector<Shader>& shaders, const vk::Extent2D& extent, const RenderPass& render_pass, vk::ArrayProxy<vk::DescriptorSetLayoutBinding> bindings, const vk::VertexInputBindingDescription& vertex_binding, const std::vector<vk::VertexInputAttributeDescription>& vertex_attrs, enum Type type, bool wireframe, bool culling)
	: dev(dev), shaders(shaders), extent(extent), render_pass(render_pass), bindings(bindings), vertex_binding(vertex_binding), vertex_attrs(vertex_attrs), type(type) {
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
	shader_info.reserve(shaders.size());
	for (const auto& shader : shaders)
		shader_info.push_back(shader.info());

	/* vertex input setup */
	vertex_bindings = {
		vertex_binding,
	};

	vertex_input_info = vk::PipelineVertexInputStateCreateInfo {
		.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_bindings.size()),
		.pVertexBindingDescriptions = vertex_bindings.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attrs.size()),
		.pVertexAttributeDescriptions = vertex_attrs.data(),
	};

	input_asm_info = vk::PipelineInputAssemblyStateCreateInfo {
		/* matters later if we use strip primitives */
		.primitiveRestartEnable = vk::False,
	};

	switch (type) {
		case eVERTEX:
		case eGLTF:
		case eBSP:
			input_asm_info.topology = vk::PrimitiveTopology::eTriangleList;
		break;
		case eTERRAIN:
			input_asm_info.topology = vk::PrimitiveTopology::ePatchList;
		break;
		case eBOX:
			input_asm_info.topology = vk::PrimitiveTopology::ePointList;
		break;
	}

	const vk::PipelineTessellationStateCreateInfo* ptesselation_info = nullptr;

	tess_info = vk::PipelineTessellationStateCreateInfo {
		/* quads*/
		.patchControlPoints = 4,
	};

	if(type == Type::eTERRAIN) {
		ptesselation_info = &tess_info;
	}

	raster_info = vk::PipelineRasterizationStateCreateInfo {
		.depthClampEnable = vk::False,
		.polygonMode = (type == eBOX || wireframe) ? vk::PolygonMode::eLine : vk::PolygonMode::eFill,
		.cullMode = (type == eBOX || !culling) ? vk::CullModeFlagBits::eNone : vk::CullModeFlagBits::eBack,
		.frontFace = vk::FrontFace::eCounterClockwise,
		.depthBiasEnable = type == eBOX,
		.depthBiasConstantFactor = 0.01,
		.lineWidth = 1.0,
	};

	multisample_info = vk::PipelineMultisampleStateCreateInfo {
		.rasterizationSamples = vk::SampleCountFlagBits::e1,
		.sampleShadingEnable = vk::False,
	};

	depth_stencil_info = vk::PipelineDepthStencilStateCreateInfo{
		.depthTestEnable = vk::True,
		.depthWriteEnable = type != eBOX,
		.depthCompareOp = vk::CompareOp::eLess,
		.depthBoundsTestEnable = vk::False,
		.stencilTestEnable = vk::False,
		.minDepthBounds = 0.0,
		.maxDepthBounds = 1.0,
	};
	
	color_blend_attachment = vk::PipelineColorBlendAttachmentState {
		/* only the box has blending */
		.blendEnable = type == eBOX,
		.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
		.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
		.colorBlendOp = vk::BlendOp::eMax,
		.srcAlphaBlendFactor = vk::BlendFactor::eOne,
		.dstAlphaBlendFactor = vk::BlendFactor::eZero,
		.alphaBlendOp = vk::BlendOp::eAdd,
		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eB
						| vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eA,
		
	};

	blend_constants = {0.0f, 0.0f, 0.0f, 0.0f };
	color_blend_info = vk::PipelineColorBlendStateCreateInfo{
		.logicOpEnable = vk::False,
		.logicOp = vk::LogicOp::eCopy,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment,
		.blendConstants = blend_constants,
	};

	/* temporary viewport and scissor, since it is a dynamic state due to the existence of window resizing */
	viewport = vk::Viewport{
		.x = 0.0,
		.y = static_cast<float>(extent.height),
		.width = static_cast<float>(extent.width),
		.height = -static_cast<float>(extent.height),
	};

	scissor = vk::Rect2D {
		.offset = 0,
		.extent = extent,
	};

	viewport_info = vk::PipelineViewportStateCreateInfo {
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor,
	};

	dyn_states[0] = vk::DynamicState::eScissor;
	dyn_states[1] = vk::DynamicState::eViewport;

	dyn_info = vk::PipelineDynamicStateCreateInfo {
		.dynamicStateCount = (uint32_t)std::size(dyn_states),
		.pDynamicStates = dyn_states,
	};

	pipeline_info = vk::GraphicsPipelineCreateInfo {
		.stageCount = static_cast<uint32_t>(shaders.size()),
		.pStages = shader_info.data(),
		.pVertexInputState = &vertex_input_info,
		.pInputAssemblyState = &input_asm_info,
		.pTessellationState = ptesselation_info,
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

void GraphicsPipeline::update(uint32_t binding, const Texture& tex) {
	auto tex_info = vk::DescriptorImageInfo {
		.sampler = tex.sampler,
		.imageView = tex.view,
		.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
	};

	dev.updateDescriptorSets(vk::WriteDescriptorSet {
		.dstSet = desc_set,
		.dstBinding = binding,
		.descriptorCount = 1,
		.descriptorType = vk::DescriptorType::eCombinedImageSampler,
		.pImageInfo = &tex_info,
	}, nullptr);
}

void GraphicsPipeline::rebuild(bool wireframe, bool culling) {
	vertex_input_info = vk::PipelineVertexInputStateCreateInfo {
		.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_bindings.size()),
		.pVertexBindingDescriptions = vertex_bindings.data(),
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attrs.size()),
		.pVertexAttributeDescriptions = vertex_attrs.data(),
	};

	raster_info.cullMode = culling ? vk::CullModeFlagBits::eBack : vk::CullModeFlagBits::eNone;
	raster_info.polygonMode = wireframe ? vk::PolygonMode::eLine : vk::PolygonMode::eFill;
	auto res = dev.createGraphicsPipeline(nullptr, pipeline_info);
	if (res.result != vk::Result::eSuccess) {
		Log::error("Failed to create pipeline: (Vulkan error code: %d)\n", res.result);
	}

	defunct_pipelines.push_back(pipeline);

	pipeline = res.value;
}

GraphicsPipeline::~GraphicsPipeline() {
	for(auto& p : defunct_pipelines) {
		dev.destroyPipeline(p);
	}
	dev.destroyDescriptorSetLayout(desc_layout);
	dev.destroyPipelineLayout(layout);
	dev.destroyDescriptorPool(desc_pool);
	dev.destroyPipeline(pipeline);
}