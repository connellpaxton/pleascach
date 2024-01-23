#include <Resources/Texture.hpp>
#include <Memory/Buffer.hpp>
#include <Memory/Memory.hpp>

#include <Renderer/CommandBuffer.hpp>

#include <util/file.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Texture::Texture(vk::PhysicalDevice phys_dev, vk::Device dev, CommandBuffer command_buffer, const std::string& fname) {
	int n_channels;
	vk::Extent3D extent;

	auto image_data = stbi_load(fname.c_str(), reinterpret_cast<int*>(&extent.width), reinterpret_cast<int*>(&extent.height), &n_channels, STBI_rgb_alpha);
	extent.depth = 1;

	image = std::make_unique<Image>(phys_dev, dev, extent, vk::Format::eR8G8B8A8Srgb,
									vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eSampled,
									vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	vk::DeviceSize sz = extent.width * extent.height * sizeof(uint32_t);

	/* staging buffer to hold image data from the CPU */
	Buffer staging(phys_dev, dev, sz,  vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	staging.map(image_data);
	stbi_image_free(image_data);

	command_buffer.copy(staging, *image);
}