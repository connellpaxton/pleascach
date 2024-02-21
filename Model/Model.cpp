#include <Model/Model.hpp>

#include <util/log.hpp>
#include <util/file.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tinygltf/tiny_gltf.h>


#include <glm/gtc/type_ptr.hpp>

Model::Model(vk::PhysicalDevice phys_dev, vk::Device dev, const std::string& fname) {
	Log::debug("Loading model " + fname + "\n");

	tinygltf::TinyGLTF loader;
	std::string err, warn;

	model = std::make_shared<tinygltf::Model>();

	auto ret = loader.LoadASCIIFromFile(model.get(), &err, &warn, fname);

	if(!warn.empty()) {
		Log::debug(fname + ": " + warn);
	}

	if(!err.empty()) {
		Log::debug(fname + ": " + err);
	}

	if(!ret) {
		Log::error("Failed to pase glTF model\n");
	}

	for(auto& node: model->nodes)
		initNode(node, nullptr);
	
	/* vertex, index buffer should be populated now */
	Log::debug("%zu vertices loaded from model " + fname + "\n", vertices.size());
	vertex_buffer = std::make_unique<VertexBuffer>(phys_dev, dev, vertices.size());
	vertex_buffer->upload(vertices);

	index_buffer = std::make_unique<Buffer>(phys_dev, dev, indices.size()*sizeof(uint16_t),
		vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);
	index_buffer->upload(reinterpret_cast<uint8_t*>(indices.data()), static_cast<vk::DeviceSize>(indices.size()*sizeof(uint16_t)));
}

void Model::initVertices(Node* node, const tinygltf::Primitive& prim) {
	Log::debug("Loading vertices\n");
	uint32_t first_idx = indices.size();
	uint32_t vert_start = vertices.size();
	uint32_t idx_count = 0;

	const float* pos_buff = nullptr;
	const float* norm_buff = nullptr;
	const float* uv_buff = nullptr;
	size_t vertex_count = 0;

	auto loc = prim.attributes.find("POSITION");
	if(loc != prim.attributes.end()) {
		auto& accessor = model->accessors[loc->second];
		const auto& view = model->bufferViews[accessor.bufferView];
		pos_buff = reinterpret_cast<const float*>(&model->buffers[view.buffer].data[accessor.byteOffset+view.byteOffset]);
		vertex_count = accessor.count;
	}
	loc = prim.attributes.find("NORMAL");
	if(loc != prim.attributes.end()) {
		auto& accessor = model->accessors[loc->second];
		const auto& view = model->bufferViews[accessor.bufferView];
		norm_buff = reinterpret_cast<const float*>(&model->buffers[view.buffer].data[accessor.byteOffset+view.byteOffset]);
	}
	loc = prim.attributes.find("TEXCOORD_0");
	if(loc != prim.attributes.end()) {
		auto& accessor = model->accessors[loc->second];
		const auto& view = model->bufferViews[accessor.bufferView];
		uv_buff = reinterpret_cast<const float*>(&model->buffers[view.buffer].data[accessor.byteOffset+view.byteOffset]);
	}

	for(size_t i = 0; i < vertex_count; i++) {
		vertices.push_back(glTFVertex {
			.pos = pos_buff? glm::make_vec3(pos_buff+i*3) : glm::vec3(0.0),
			.norm = norm_buff? glm::normalize(glm::make_vec3(norm_buff+i*3)) : glm::vec3(0.0),
			.uv = uv_buff? glm::make_vec2(uv_buff+i*2) : glm::vec2(0.0),
			.color = glm::vec3(0.0),
		});
	}

	Log::debug("Finished loading %zu vertices\n", vertex_count);

	const auto& accessor = model->accessors[prim.indices];
	const auto& view = model->bufferViews[accessor.bufferView];
	const auto& buffer = model->buffers[view.buffer];

	idx_count += accessor.count;

	switch(accessor.componentType) {
		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
			for(size_t i = 0; i < accessor.count; i++)
				indices.push_back(
					reinterpret_cast<const uint32_t*>(
						&buffer.data[accessor.byteOffset+view.byteOffset
					])[i]
					+ vert_start
				);
		break;
		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
			for(size_t i = 0; i < accessor.count; i++)
				indices.push_back(
					reinterpret_cast<const uint16_t*>(
						&buffer.data[accessor.byteOffset+view.byteOffset
					])[i]
					+ vert_start
				);
		break;
		case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
			for(size_t i = 0; i < accessor.count; i++)
				indices.push_back(
					reinterpret_cast<const uint8_t*>(
						&buffer.data[accessor.byteOffset+view.byteOffset
					])[i]
					+ vert_start
				);
		break;
		default:
			Log::error("Unrecognized index type in model\n");
		break;
	}

	Log::debug("Loaded %zu indices of type %d\n", accessor.count, (accessor.componentType-TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)/2+1);

	node->mesh.push_back(Primitive {
		.first_idx = first_idx,
		.idx_count = idx_count,
	});
}

void Model::initNode(const tinygltf::Node& node, Node* parent, int level) {
	Log::debug("%*sNode:"+node.name+"\n", level*4, " ");

	Node* ret = new Node;

	ret->parent = parent;

	/* load kids */
	for(auto& child_idx : node.children) {
		initNode(model->nodes[child_idx], ret, level+1);
	}

	if(node.mesh >= 0) {
		const auto& mesh = model->meshes[node.mesh];
		for(auto& prim : mesh.primitives) {
			initVertices(ret, prim);
		}
	}

	if(ret->parent)
		ret->parent->children.push_back(ret);
	else
		nodes.push_back(ret);
}