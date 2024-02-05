#include <Resources/Texture.hpp>
#include <Memory/Buffer.hpp>
#include <Memory/Memory.hpp>

#include <Renderer/CommandBuffer.hpp>

#include <util/file.hpp>
#include <util/log.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

/* externall synchonized, just writes to a command buffer, you still need to pull the trigger (allows bulk texture image preperation) */
Texture::Texture(vk::PhysicalDevice phys_dev, vk::Device dev, CommandBuffer& command_buffer, const std::string& fname, bool free_memory) : dev(dev), free_memory(free_memory) {
	Log::debug("Starting texture creation for: %s\n", fname.c_str());

	int n_channels;


	image_data = stbi_load(fname.c_str(), reinterpret_cast<int*>(&extent.width), reinterpret_cast<int*>(&extent.height), &n_channels, STBI_rgb_alpha);
	Log::info("Texture is %dx%d loaded at %p\n", extent.width, extent.height, image_data);
	extent.depth = 1;

	image = std::make_unique<Image>(phys_dev, dev, extent, vk::Format::eR8G8B8A8Unorm,
									vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
									vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	vk::DeviceSize sz = static_cast<vk::DeviceSize>(extent.width * extent.height) * sizeof(uint32_t);

	/* staging buffer to hold image data from the CPU */
	 staging = std::make_unique<Buffer>(phys_dev, dev, sz,  vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	staging->upload(image_data);
	if(free_memory) {
		stbi_image_free(image_data);
		image_data = nullptr;
	}

	/* pipeline memory barrier ensures this doesn't get reordered wrong */

	auto staging_buffer_trans_barrier = vk::ImageMemoryBarrier {
		.srcAccessMask = vk::AccessFlagBits(0),
		.dstAccessMask = vk::AccessFlagBits::eTransferWrite,
		.oldLayout = vk::ImageLayout::eUndefined,
		.newLayout = vk::ImageLayout::eTransferDstOptimal,
		.image = *image,
		.subresourceRange = vk::ImageSubresourceRange {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.levelCount = 1,
			.layerCount = 1,
		},
	};

	/* transfer optimal --> shader optimal */
	auto shader_barrier = vk::ImageMemoryBarrier{
		.srcAccessMask = vk::AccessFlagBits::eTransferWrite,
		.dstAccessMask = vk::AccessFlagBits::eShaderRead,
		.oldLayout = vk::ImageLayout::eTransferDstOptimal,
		.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
		.image = *image,
		.subresourceRange = vk::ImageSubresourceRange {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.levelCount = 1,
			.layerCount = 1,
		},
	};
	
	command_buffer.command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits(0), nullptr, nullptr, staging_buffer_trans_barrier);
	command_buffer.copy(*staging, *image);
	command_buffer.command_buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlagBits(0), nullptr, nullptr, shader_barrier);
}


void Texture::finishCreation() {
	view = dev.createImageView(vk::ImageViewCreateInfo {
		.image = *image,
		.viewType = vk::ImageViewType::e2D,
		.format = vk::Format::eR8G8B8A8Unorm,
		.subresourceRange = vk::ImageSubresourceRange{
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.levelCount = 1,
			.layerCount = 1,
		},
	});

	sampler = dev.createSampler(vk::SamplerCreateInfo {
		.magFilter = vk::Filter::eLinear,
		.minFilter = vk::Filter::eLinear,
		.mipmapMode = vk::SamplerMipmapMode::eLinear,
		.addressModeU = vk::SamplerAddressMode::eMirroredRepeat,
		.addressModeV = vk::SamplerAddressMode::eMirroredRepeat,
		.addressModeW = vk::SamplerAddressMode::eMirroredRepeat,
		.mipLodBias = 0.0,
		.anisotropyEnable = vk::False,
		.maxAnisotropy = 0.0,
		.compareEnable = vk::False,
		.compareOp = vk::CompareOp::eAlways,
		.minLod = 0.0,
		.maxLod = 0.0,
		.borderColor = vk::BorderColor::eIntOpaqueBlack,
		.unnormalizedCoordinates = vk::False,
	});
}

void Texture::cleanup() {
	if (!free_memory)
		free(image_data);
	staging.reset();
	image->cleanup();
	dev.destroyImageView(view);
	dev.destroySampler(sampler);
}