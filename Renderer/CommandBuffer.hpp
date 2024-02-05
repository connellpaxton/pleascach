#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

#include <util/int.hpp>

#include <memory>

struct Buffer;
struct Image;
struct GraphicsPipeline;
struct ComputePipeline;
struct VertexBuffer;
struct Model;
struct Terrain;

struct CommandBuffer {
	CommandBuffer(vk::Device dev, u32 queue_family);

	void cleanup(vk::Device dev);

	/* start recording commands */
	void begin();

	/* copy between buffer */
	void copy(vk::Buffer in, vk::Buffer out, vk::ArrayProxy<const vk::BufferCopy> regions);
	/* copy buffer to image */
	void copy(Buffer& in, Image& out, vk::ImageLayout layout = vk::ImageLayout::eTransferDstOptimal);

	void bind(const GraphicsPipeline& pipeline);
	void bind(vk::PipelineLayout layout, vk::ArrayProxy<vk::DescriptorSet> desc_sets);
	void bind(const VertexBuffer& vertex_buffer, uint32_t binding = 0);
	void bind(std::shared_ptr<Model> model);
	void bind(Terrain* terrain);

	void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex = 0, uint32_t first_instance = 0);

	/* stop recording commands */
	void end();

	void recycle();

	vk::CommandBuffer command_buffer;
	vk::CommandPool command_pool;

	operator vk::CommandBuffer* () {
		return &command_buffer;
	}
};
