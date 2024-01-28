#include <Memory/Image.hpp>
#include <Memory/Memory.hpp>


Image::Image(vk::PhysicalDevice phys_dev, vk::Device dev, vk::Image _image, vk::MemoryPropertyFlags memory_flags, vk::Extent3D& extent) : dev(dev), image(_image) {

	auto reqs = dev.getImageMemoryRequirements(image);

	auto alloc = vk::MemoryAllocateInfo{
		.allocationSize = reqs.size,
		.memoryTypeIndex = mem::choose_heap(phys_dev, reqs, memory_flags),
	};
	memory = dev.allocateMemory(alloc);
	dev.bindImageMemory(image, memory, 0);
	this->extent = extent;
	Log::debug("Image memory: %p\n", memory);

}

Image::Image(vk::PhysicalDevice phys_dev, vk::Device dev, vk::ImageCreateInfo info, vk::MemoryPropertyFlags memory_flags) : dev(dev) {
	image = dev.createImage(info);
	*this = Image(phys_dev, dev, image, memory_flags, info.extent);
}

Image::Image(vk::PhysicalDevice phys_dev, vk::Device dev, vk::Extent3D extent, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags memory_flags) : dev(dev) {
	auto info = vk::ImageCreateInfo{
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent = extent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive,
		.initialLayout = vk::ImageLayout::eUndefined,
	};

	*this = Image(phys_dev, dev, info, memory_flags);
}

void Image::cleanup() {
	dev.destroyImage(image);
	dev.freeMemory(memory);
}