#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Memory/Image.hpp>

struct CommandBuffer;

struct Texture {
	std::unique_ptr<Image> image;
	Texture(vk::PhysicalDevice phys_dev, vk::Device dev, CommandBuffer command_buffer, const std::string& fname);
};