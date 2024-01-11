#include <Renderer/Texture.hpp>
#include <Renderer/CommandBuffer.hpp>

#include <vkb/VkBootstrap.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <string>
using namespace std::string_literals;

Texture::Texture(vkb::Device& dev, VmaAllocator& allocator, const std::string& fname) : fname(fname) {
	unsigned int width;
	unsigned int height;
	int n_channels;

	uint8_t* data = stbi_load(fname.c_str(), reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height), &n_channels, STBI_rgb_alpha);

	if (!data) {
		stbi_image_free(data);
		throw "Failed to load texture \""s + fname + "\": " + stbi_failure_reason();
	}

	vk::DeviceSize image_size = width * height * sizeof(uint32_t);

	vk::Extent3D img_ext {
		.width = width,
			.height = height,
			.depth = 1,
	};

	vk::ImageCreateInfo image_info {
		.imageType = vk::ImageType::e2D,
		.format = vk::Format::eR8G8B8A8Unorm,
		.extent = img_ext,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = vk::ImageTiling::eLinear,
		.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		.sharingMode = vk::SharingMode::eExclusive,
		.initialLayout = vk::ImageLayout::eUndefined,
	};

	VmaAllocationCreateInfo alloc_info {
		.usage = VMA_MEMORY_USAGE_CPU_ONLY,
	};
	
	VkImage tmp_img;

	if (vmaCreateImage(allocator, &image_info.operator const VkImageCreateInfo & (), &alloc_info, &tmp_img, &alloc, NULL)) {
		throw "Failed to create image for "s + fname;
	}

	image = vk::Image(tmp_img);

	vk::BufferCreateInfo staging_buffer_info {
		.size = image_size,
		.usage = vk::BufferUsageFlagBits::eTransferSrc,
	};

	vk::Buffer staging_buffer;
	VmaAllocation staging_alloc;

	VmaAllocationCreateInfo staging_alloc_info{
		.usage = VMA_MEMORY_USAGE_CPU_ONLY,
	};


	VkBuffer tmp_buff;
	if (vmaCreateBuffer(allocator, &staging_buffer_info.operator const VkBufferCreateInfo & (), &staging_alloc_info, &tmp_buff, &staging_alloc, NULL) != VK_SUCCESS) {
		throw "Failed to allocate texture staging buffer for "s + fname;
	}

	staging_buffer = vk::Buffer(tmp_buff);

	void* staging_data;
	vmaMapMemory(allocator, staging_alloc, &staging_data);
	std::memcpy(staging_data, data, static_cast<uint32_t>(image_size));
	vmaUnmapMemory(allocator, staging_alloc);

	stbi_image_free(data);

	vk::ImageSubresourceRange staging_range {
		.aspectMask = vk::ImageAspectFlagBits::eColor,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1,
	};

	/* memory barrier for undefined->transfer optimal */
	vk::ImageMemoryBarrier staging_memory_barrier {
		.srcAccessMask = vk::AccessFlagBits::eNone,
		.dstAccessMask = vk::AccessFlagBits::eTransferWrite,
		.oldLayout = vk::ImageLayout::eUndefined,
		.newLayout = vk::ImageLayout::eTransferDstOptimal,
		.image = image,
		.subresourceRange = staging_range,
	};

	vk::BufferImageCopy staging_buffer_copy {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource = vk::ImageSubresourceLayers {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
		.imageExtent = img_ext,
	};

	/* transfer optimal -> shader optimal */
	vk::ImageMemoryBarrier staging_buffer_to_shader_barrier {
		.srcAccessMask = vk::AccessFlagBits::eTransferWrite,
		.dstAccessMask = vk::AccessFlagBits::eShaderRead,
		.oldLayout = vk::ImageLayout::eTransferDstOptimal,
		.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
		.image = image,
		.subresourceRange = staging_range,
	};

	CommandBuffer cmd(dev);

	vk::CommandBufferBeginInfo cmd_begin_info { .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit	};

	cmd.buffer.begin(cmd_begin_info);
	cmd.buffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,);

	/* TODO: Finish transfer */
}