#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Renderer/CommandBuffer.hpp>

#include <vector>

struct Object;
struct Renderer;
struct Camera;

struct UI {
	struct UI_Info {
		float fps = 0.0;
		bool& flycam;
		float& time;
		/* for cantor */
		float& rad;
		/* camera stuff */
		Camera& cam;
		const std::vector<Object>& objects;
	} info;

	vk::Device dev;
	vk::DescriptorPool desc_pool;

	UI(Renderer* ren);

	void newFrame();
	void render(vk::CommandBuffer cmd);
	
	~UI();
};