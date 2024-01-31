#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Renderer/CommandBuffer.hpp>

struct Renderer;
struct Camera;

struct UI {
	struct UI_Info {
		float fps = 0.0;
		bool& flycam;
		/* camera stuff */
		Camera& cam;
	} info;

	vk::Device dev;
	vk::DescriptorPool desc_pool;

	UI(Renderer* ren);

	void newFrame();
	void render(vk::CommandBuffer cmd);
	
	~UI();
};