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
		bool& visibility_testing;
		float& time;
		/* camera stuff */
		Camera& cam;
		float& tess_factor;
		float& tess_edge_size;
		const size_t& n_indices;
	} info;

	vk::Device dev;
	vk::DescriptorPool desc_pool;

	UI(Renderer* ren);

	void newFrame();
	void render(vk::CommandBuffer cmd);
	
	~UI();
};