#include <Renderer/RenderPass.hpp>

RenderPass::RenderPass(vk::Device dev, vk::Format image_format, vk::Format depth_format) {
	/* transform color image from UNDEFINED format to presentable one for rendering */
	auto color_attach_desc = vk::AttachmentDescription {
		.format = image_format,
		.samples = vk::SampleCountFlagBits::e1,
		/* since its set to clear, it clears the initial image to the specified 
			clear color instead of loading the data */
		.loadOp = vk::AttachmentLoadOp::eClear,
		/* we care about the data after the end of the renderpass, so store */
		.storeOp = vk::AttachmentStoreOp::eStore,
		/* for the depth stencil buffer, we care about neither */
		.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
		.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		.initialLayout = vk::ImageLayout::eUndefined,
		.finalLayout = vk::ImageLayout::ePresentSrcKHR,
	};

	auto color_attach_ref = vk::AttachmentReference { .attachment = 0, .layout = vk::ImageLayout::eColorAttachmentOptimal };

	auto depth_attach_desc = vk::AttachmentDescription{
		.format = depth_format,
		.samples = vk::SampleCountFlagBits::e1,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eDontCare,
		.stencilLoadOp = vk::AttachmentLoadOp::eClear,
		.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		.initialLayout = vk::ImageLayout::eUndefined,
		.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
	};

	auto depth_attach_ref = vk::AttachmentReference{ .attachment = 1, .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal };

	/* only one subpass for now, uses both attachments */
	auto subpass_desc = vk::SubpassDescription {
		.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attach_ref,
		.pDepthStencilAttachment = &depth_attach_ref,
	};


	/* designates producer and consumer of the image to position subpass */
	auto color_dep = vk::SubpassDependency{
		.srcSubpass = vk::SubpassExternal,
		.dstSubpass = 0,
		.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
		.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
		.srcAccessMask = vk::AccessFlagBits::eNone,
		.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
	};

	auto depth_dep = vk::SubpassDependency {
		.srcSubpass = vk::SubpassExternal,
		.dstSubpass = 0,
		.srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
		.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
		.srcAccessMask = vk::AccessFlagBits::eNone,
		.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite,
	};

	vk::AttachmentDescription descs[] = { color_attach_desc, depth_attach_desc };
	vk::SubpassDependency deps[] = { color_dep, depth_dep};
	vk::SubpassDescription subpasses[] = {subpass_desc};

	auto pass_info = vk::RenderPassCreateInfo {
		.attachmentCount = static_cast<uint32_t>(std::size(descs)),
		.pAttachments = descs,
		.subpassCount = static_cast<uint32_t>(std::size(subpasses)),
		.pSubpasses = subpasses,
		.dependencyCount = static_cast<uint32_t>(std::size(deps)),
		.pDependencies = deps,
	};

	render_pass = dev.createRenderPass(pass_info);
}

void RenderPass::cleanup(vk::Device dev) {
	dev.destroyRenderPass(render_pass);
}