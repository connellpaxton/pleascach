#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Renderer/CommandBuffer.hpp>

struct Renderer;

struct UI {
	struct UI_Info {
		float fps = 0.0;
	} info;

	vk::Device dev;
	vk::DescriptorPool desc_pool;

	UI(Renderer* ren);

	void newFrame();
	void render(vk::CommandBuffer cmd);
	
	~UI();
};