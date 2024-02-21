#pragma once

#include <Resources/Texture.hpp>
#include <Renderer/VertexBuffer.hpp>

#include <Memory/Buffer.hpp>

#include <string>

struct CommandBuffer;

struct Terrain {
	vk::PhysicalDevice phys_dev;
	vk::Device dev;
	Texture* heightmap_tex;
	
	std::unique_ptr<VertexBuffer> vertex_buffer;
	std::unique_ptr<Buffer> index_buffer;
	std::vector<glTFVertex> vertices;
	std::vector<uint32_t> indices;

	Terrain(vk::PhysicalDevice phys_dev, vk::Device dev, Texture& hieghtmap);
	float getHeight(int32_t x, int32_t y);
	void draw(CommandBuffer& cmd);
	~Terrain();
};