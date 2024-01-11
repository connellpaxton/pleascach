#include <Renderer/RenderPass.hpp>
#include <Renderer/Renderer.hpp>

RenderPass::RenderPass(vk::Device dev, Swapchain* swp) : dev(dev) {
	vk::AttachmentDescription color_attr {
		.format = vk::Format(swp->swapchain.image_format),
		.samples = vk::SampleCountFlagBits::e1,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
		.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		.initialLayout = vk::ImageLayout::eUndefined,
		.finalLayout = vk::ImageLayout::ePresentSrcKHR,
	};

	vk::AttachmentReference color_attr_ref {
		.attachment = 0,
		.layout = vk::ImageLayout::eColorAttachmentOptimal
	};

	vk::AttachmentDescription depth_attr {
		.format = vk::Format::eD32Sfloat,
		.samples = vk::SampleCountFlagBits::e1,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.stencilLoadOp = vk::AttachmentLoadOp::eClear,
		.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		.initialLayout = vk::ImageLayout::eUndefined,
		.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
	};

	vk::AttachmentReference depth_attr_ref {
		.attachment = 1,
			.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
	};

	vk::SubpassDescription subpass_desc {
		.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attr_ref,
		.pDepthStencilAttachment = &depth_attr_ref,
	};

	vk::SubpassDependency subpass_dep {
		.srcSubpass = vk::SubpassExternal,
		.dstSubpass = 0,
		.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
		.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
		.srcAccessMask = vk::AccessFlagBits(0),
		.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite,
	};

	vk::SubpassDependency depth_dep {
		.srcSubpass = vk::SubpassExternal,
		.dstSubpass = 0,
		.srcStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
		.dstStageMask = vk::PipelineStageFlagBits::eEarlyFragmentTests | vk::PipelineStageFlagBits::eLateFragmentTests,
		.srcAccessMask = vk::AccessFlagBits(0),
		.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite,
	};

	vk::SubpassDependency deps[] = { subpass_dep, depth_dep };
	vk::AttachmentDescription attachments[] = { color_attr, depth_attr };

	vk::RenderPassCreateInfo render_pass_info {
		.attachmentCount = 2,
		.pAttachments = attachments,
		.subpassCount = 1,
		.pSubpasses = &subpass_desc,
		.dependencyCount = 2,
		.pDependencies = deps,
	};

	render_pass = dev.createRenderPass(render_pass_info);
}

RenderPass::~RenderPass() {
	dev.destroyRenderPass(render_pass);
}