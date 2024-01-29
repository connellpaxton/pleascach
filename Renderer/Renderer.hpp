#pragma once

#include <memory>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Renderer/Swapchain.hpp>
#include <Renderer/CommandBuffer.hpp>
#include <Renderer/RenderPass.hpp>

#include <Model/Model.hpp>

#include <UI/UI.hpp>

struct Window;
struct UniformBuffer;
struct VertexBuffer;
struct Texture;

/* Contains all of the Vulkan objects involved in rendering the scene */
struct Renderer {
	Renderer(Window& win);
	~Renderer();

	std::vector<Texture> createTextures(const std::vector<std::string>& names);

	void draw();
	void present();

	Window& win;

	vk::Instance instance;
	vk::PhysicalDevice phys_dev;
	vk::Device dev;

	vk::Fence render_fence;
	vk::Semaphore image_wait_semaphore, render_wait_semaphore;
	vk::SurfaceKHR surface;
	std::unique_ptr<Swapchain> swapchain;

	int queue_family;
	vk::Queue queue;

	std::unique_ptr<CommandBuffer> command_buffer;
	std::unique_ptr<RenderPass> render_pass;

	/* For now, couple it all together as one pipeline with one uniform buffer, vertex buffer, etc */
	std::unique_ptr<GraphicsPipeline> pipeline;
	std::unique_ptr<VertexBuffer> vertex_buffer;
	std::unique_ptr<UniformBuffer> uniform_buffer;

	std::vector<Texture> textures;

	uint32_t current_image_idx;
	uint64_t frame = 0;

	std::vector<std::shared_ptr<Model>> models;
	std::unique_ptr<UI> ui;
};
