#include <Renderer/Pipeline.hpp>
#include <Renderer/VertexInfo.hpp>
#include <Renderer/Swapchain.hpp>
#include <Renderer/RenderPass.hpp>

#include <string>

using namespace std::string_literals;

Pipeline::Pipeline
(const std::vector <vk::DescriptorSetLayout>& layouts,
	const std::shared_ptr<Shader> frag_shader,
	const std::shared_ptr<Shader> vert_shader, vk::Device& dev, const VertexInfo& vertex_info,
	const Swapchain& swp, const RenderPass& render_pass, const vk::PrimitiveTopology topo, bool enable_depth) :
	frag_shader(frag_shader), vert_shader(vert_shader), dev(dev) {

	vk::PipelineLayoutCreateInfo layout_info {
		.setLayoutCount = 1,
			.pSetLayouts = layouts.data(),
			.pushConstantRangeCount = 0,
	};

	layout = dev.createPipelineLayout(layout_info);

	vk::VertexInputBindingDescription main_binding {
		.binding = 0,
		.stride = static_cast<uint32_t>(vertex_info.size()),
		.inputRate = vk::VertexInputRate::eVertex,
	};

	auto descs = vertex_info.create_descs();

	vk::PipelineVertexInputStateCreateInfo input_info {
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &main_binding,
		.vertexAttributeDescriptionCount = static_cast<uint32_t>(descs.size()),
		.pVertexAttributeDescriptions = descs.data(),
	};

	vk::PipelineInputAssemblyStateCreateInfo asm_info {
		.topology = topo,
		.primitiveRestartEnable = VK_FALSE,
	};

	vk::Viewport viewport {
		.x = 0.0,
		.y = 0.0,
		.width = static_cast<float>(swp.swapchain.extent.width),
		.height = static_cast<float>(swp.swapchain.extent.height),
		.minDepth = 0.0,
		.maxDepth = 1.0,
	};

	vk::Rect2D scissor {
		.offset = { 0, 0 },
		.extent = { swp.swapchain.extent.height, swp.swapchain.extent.height}
	};

	vk::PipelineViewportStateCreateInfo viewport_info {
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor,
	};

	vk::PipelineRasterizationStateCreateInfo raster_info {
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = vk::PolygonMode::eFill,
		.cullMode = vk::CullModeFlagBits::eBack,
		.frontFace = vk::FrontFace::eClockwise,
		.depthBiasEnable = VK_FALSE,
		.lineWidth = 1.0,
	};

	vk::PipelineMultisampleStateCreateInfo multisample_info {
		.rasterizationSamples = vk::SampleCountFlagBits::e1,
		.sampleShadingEnable = VK_FALSE,
	};

	vk::PipelineColorBlendAttachmentState color_blend_attachment {
		.blendEnable = VK_FALSE,
		.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
						| vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
	};

	vk::PipelineColorBlendStateCreateInfo color_blend_info {
		.logicOpEnable = VK_FALSE,
		.logicOp = vk::LogicOp::eCopy,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment,
		.blendConstants = std::array<float, 4>({0.0, 0.0, 0.0, 0.0}),
	};

	vk::PipelineDepthStencilStateCreateInfo depth_stencil_info {
		.depthTestEnable = enable_depth,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = vk::CompareOp::eLessOrEqual,
		.depthBoundsTestEnable = VK_FALSE,
		.stencilTestEnable = VK_FALSE,
		.minDepthBounds = 0.0,
		.maxDepthBounds = 1.0,
	};

	vk::DynamicState dyn_states[] = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

	vk::PipelineDynamicStateCreateInfo dyn_info {
		.dynamicStateCount = static_cast<uint32_t>(std::size(dyn_states)),
		.pDynamicStates = dyn_states,
	};

	vk::PipelineShaderStageCreateInfo stages[] = {vert_shader->stage_info, frag_shader->stage_info};

	vk::GraphicsPipelineCreateInfo pipeline_info {
		.stageCount = 2,
		.pStages = stages,
		.pVertexInputState = &input_info,
		.pInputAssemblyState = &asm_info,
		.pViewportState = &viewport_info,
		.pRasterizationState = &raster_info,
		.pMultisampleState = &multisample_info,
		.pDepthStencilState = &depth_stencil_info,
		.pColorBlendState = &color_blend_info,
		.pDynamicState = &dyn_info,
		.layout = layout,
		.renderPass = render_pass.render_pass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
	};

	auto res = dev.createGraphicsPipeline(VK_NULL_HANDLE, pipeline_info);
	if (res.result != vk::Result::eSuccess) {
		throw "Failed to create pipeline"s;
	}
}

Pipeline::~Pipeline() {
	dev.destroyPipeline(pipeline, NULL);
	dev.destroyPipelineLayout(layout, NULL);
}