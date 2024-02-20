#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Renderer/CommandBuffer.hpp>

#include <imgui/imgui_console.h>

#include <memory>

struct Renderer;
struct Camera;

struct UI {
	Renderer* ren;
	vk::Device dev;
	vk::DescriptorPool desc_pool;

	UI(Renderer* ren);


	std::unique_ptr<ImGuiConsole> console;
	void newFrame();
	void render(vk::CommandBuffer cmd);
	
	~UI();
};