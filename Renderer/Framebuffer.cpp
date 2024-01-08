#include <Renderer/Framebuffer.hpp>
#include <Renderer/Renderer.hpp>

Framebuffer::Framebuffer(Renderer* ren, Swapchain* swp) : ren(ren) {
	images = swp->swapchain.get_images().value();
	image_views = swp->swapchain.get_image_views().value();

	framebuffers.resize(image_views.size());
	size_t i = 0;
	for (auto& image_view : image_views) {
		vk::ImageView attachments[] = {
				image_view,
				swp->depth_image_view,
		};

		vk::FramebufferCreateInfo framebuffer_info {
			.renderPass = ren->render_pass->render_pass,
			.attachmentCount = 2,
			.pAttachments = attachments,
			.width = swp->swapchain.extent.width,
			.height = swp->swapchain.extent.height,
			.layers = 1,
		};

		framebuffers[i++] = vk::Device(ren->dev).createFramebuffer(framebuffer_info);
	}
}

Framebuffer::~Framebuffer() {
	ren->swapchain->swapchain.destroy_image_views(image_views);
	for (auto& fb : framebuffers)
		vk::Device(ren->dev).destroyFramebuffer(fb);
}