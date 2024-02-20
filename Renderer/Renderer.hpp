#pragma once

#include <memory>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <Renderer/Swapchain.hpp>
#include <Renderer/CommandBuffer.hpp>
#include <Renderer/RenderPass.hpp>

#include <Renderer/VertexBuffer.hpp>

#include <Scene/Camera.hpp>
#include <Scene/BSP.hpp>
#include <Scene/Terrain.hpp>

#include <Model/Model.hpp>

#include <UI/UI.hpp>


struct Window;
struct UniformBuffer;
struct Texture;

/* Contains all of the Vulkan objects involved in rendering the scene */
struct Renderer {
	Renderer(Window& win);
	~Renderer();

	std::vector<Texture> createResources(const std::vector<std::string>& names);

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

	std::unique_ptr<GraphicsPipeline> vertex_pipeline;
	std::unique_ptr<GraphicsPipeline> model_pipeline;
	std::unique_ptr<GraphicsPipeline> terrain_pipeline;
	std::unique_ptr<VertexBuffer> vertex_buffer;
	std::unique_ptr<UniformBuffer> uniform_buffer;

	std::unique_ptr<Q3BSP::BSP> bsp;
	std::unique_ptr<Terrain> terrain;

	std::vector<Texture> textures;

	uint32_t current_image_idx;
	uint64_t frame = 0;

	std::vector<std::shared_ptr<Model>> models;
	std::unique_ptr<UI> ui;

	Camera cam{ .pos = glm::vec3(0.0, 5.0, 0.0), };

	bool capture_mouse = false;
	bool flycam = false;
	/* time speed */
	float time = 0.0;
	float speed = 1.0;
	bool running = true;

	size_t n_indices;
	bool visibility_testing;

	float near_plane = 2.0f;
	float far_plane = 10000.0f;

	float tess_factor = 1.8f;
	float tess_edge_size = 20.0f;
};
