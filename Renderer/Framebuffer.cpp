#include <Renderer/Framebuffer.hpp>
#include <Renderer/Renderer.hpp>

Framebuffer::Framebuffer(vk::Device dev, const RenderPass* render_pass, Swapchain& swp) : dev(dev), swp(swp) {
	images = swp.swapchain.get_images().value();
	image_views = swp.swapchain.get_image_views().value();

	framebuffers.resize(image_views.size());
	size_t i = 0;
	for (auto& image_view : image_views) {
		vk::ImageView attachments[] = {
				image_view,
				swp.depth_image_view,
		};

		vk::FramebufferCreateInfo framebuffer_info {
			.renderPass = render_pass->render_pass,
			.attachmentCount = 2,
			.pAttachments = attachments,
			.width = swp.swapchain.extent.width,
			.height = swp.swapchain.extent.height,
			.layers = 1,
		};

		framebuffers[i++] = dev.createFramebuffer(framebuffer_info);
	}
}

Framebuffer::~Framebuffer() {
	swp.swapchain.destroy_image_views(image_views);
	for (auto& fb : framebuffers)
		dev.destroyFramebuffer(fb);
}