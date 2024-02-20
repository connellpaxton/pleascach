#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Renderer/CommandBuffer.hpp>

#include <imgui/imgui_console.h>

#include <memory>

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
		float& near_plane;
		float& far_plane;
		bool& paused;
	} info;



	vk::Device dev;
	vk::DescriptorPool desc_pool;

	UI(Renderer* ren);


	std::unique_ptr<ImGuiConsole> console;
	void newFrame();
	void render(vk::CommandBuffer cmd);
	
	~UI();
};