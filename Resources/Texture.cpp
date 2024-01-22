#include <Resources/Texture.hpp>
#include <Memory/Memory.hpp>

#include <util/file.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Texture::Texture(vk::Device dev, vk::PhysicalDevice phys_dev, const std::string& fname) {
	vk::Extent3D extent;
	auto image_data = stbi_load(fname.c_str(), reinterpret_cast<int*>(&extent.width), reinterpret_cast<int*>(&extent.height), NULL, NULL);
	extent.depth = 1;

	auto image_info = vk::ImageCreateInfo{
		.imageType = vk::ImageType::e2D,
		.format = vk::Format::eR8G8B8A8Unorm,
		.extent = extent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = vk::ImageTiling::eOptimal,
		.usage = vk::ImageUsageFlagBits::eSampled,
		.sharingMode = vk::SharingMode::eExclusive,
		.initialLayout = vk::ImageLayout::eUndefined,
	};

	image = dev.createImage(image_info);

	auto reqs = dev.getImageMemoryRequirements(image);


	auto image_alloc_info = vk::MemoryAllocateInfo{
		.allocationSize = reqs.size,
		.memoryTypeIndex = mem::choose_heap(phys_dev, reqs, vk::MemoryPropertyFlagBits::eHostVisible),
	};

	image_alloc = dev.allocateMemory(image_alloc_info);
	dev.bindImageMemory(image, image_alloc, 0);

	/* TODO: Copy memory into image using buffers */
}

void Texture::cleanup(vk::Device dev) {
	dev.freeMemory(image_alloc);
}