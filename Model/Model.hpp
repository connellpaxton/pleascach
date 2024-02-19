#pragma once

#include <Memory/Buffer.hpp>

#include <Renderer/VertexBuffer.hpp>

#include <memory>

#include <glm/glm.hpp>
#include <tinygltf/tiny_gltf.h>

struct Model {
	struct Primitive {
		uint32_t first_idx;
		uint32_t idx_count;
		uint32_t mat_idx;
	};

	using Mesh = std::vector<Primitive>;
	struct Node {
		Node* parent;
		std::vector<Node*> children;
		Mesh mesh;
		glm::mat4 mat;
		~Node() {
			for(auto& n : children)
				delete n;
		}
	};

	Model(vk::PhysicalDevice phys_dev, vk::Device dev, const std::string& fname);
	
	std::shared_ptr<tinygltf::Model> model;

	void initVertices(Node*, const tinygltf::Primitive&);

	std::unique_ptr<VertexBuffer> vertex_buffer;
	std::unique_ptr<Buffer> index_buffer;
	std::vector<BasicVertex> vertices;
	std::vector<uint16_t> indices;

	/* recusively initialize nodes with an accumulative vertex and index buffer collector */
	void initNode(const tinygltf::Node& node, Node* parent, int level = 0);

	std::vector<Node*> nodes;

	~Model() {
		for(auto& n : nodes)
			delete n;
		
		vertex_buffer.reset();
		index_buffer.reset();
	}	
};
