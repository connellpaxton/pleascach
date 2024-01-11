#pragma once
#include <Renderer/Shader.hpp>

struct Swapchain;
struct VertexInfo;
struct RenderPass;

struct Pipeline {
	std::shared_ptr<Shader> frag_shader, vert_shader;

	Pipeline(const std::vector <vk::DescriptorSetLayout>& layouts,
			const std::shared_ptr<Shader> frag_shader,
			const std::shared_ptr<Shader> vert_shader, vk::Device& dev,
			const VertexInfo& vertex_info, const Swapchain& swp, const RenderPass& render_pass,
			const vk::PrimitiveTopology topo = vk::PrimitiveTopology::eTriangleList,
			bool enable_depth = VK_TRUE);
	~Pipeline();

	vk::Pipeline pipeline;
	vk::PipelineLayout layout;
	vk::Device& dev;
};