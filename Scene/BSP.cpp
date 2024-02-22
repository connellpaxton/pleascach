#include <Scene/BSP.hpp>

#include <Renderer/Pipeline.hpp>

#include <util/geo.hpp>
#include <util/file.hpp>
#include <util/log.hpp>

#include <set>

#include <cstring>


using namespace HLBSP;

static inline void copy_data(void* file_data, std::string& dst, Lump& lump) {
	dst.resize(lump.len);
	std::memcpy(dst.data(), (u8*)file_data + (size_t)lump.offset, lump.len);
}

template<typename T>
static inline void copy_data(void* file_data, std::vector<T>& dst, Lump& lump) {
	Log::debug("%zu items\n", lump.len / sizeof(T));
	dst.resize(lump.len / sizeof(T));
	std::memcpy(dst.data(), ((u8*)file_data) + lump.offset, lump.len);
}

void BSP::load_indices(const glm::vec3& cam_pos, bool visibility_test, const glm::mat4& view) {
	std::set<int> present_faces;
	std::vector<Face> visible_faces;
	// if (visibility_test) {
	if(false) {
		auto leaf_idx = determine_leaf(cam_pos);

		auto fr_planes = frustum(view);

		if (leaf_idx == last_leaf)
			index_buffer->upload(indices);

		last_leaf = leaf_idx;
		auto& cam_leaf = leaves[leaf_idx];


		std::vector<Leaf> visible_leafs;
		for (auto& leaf : leaves) {

			const auto min = leaf.bb_mins;
			const auto max = leaf.bb_maxes;

			const glm::vec3 bounding_planes[8] = {
				{ min.x, min.y, min.z },
				{ max.x, min.y, min.z },
				{ max.x, max.y, min.z },
				{ min.x, max.y, min.z },
				{ min.x, min.y, max.z },
				{ max.x, min.y, max.z },
				{ max.x, max.y, max.z },
				{ min.x, max.y, max.z },
			};

			if (determine_visibility(cam_leaf, leaf, fr_planes, bounding_planes))
				visible_leafs.push_back(leaf);
		}

		for (const auto& leaf : visible_leafs) {
			for (size_t i = 0; i < leaf.n_mark_surfaces; i++) {
				auto idx = mark_surfaces[leaf.first_mark_surface_idx + i];
				if (present_faces.contains(idx))
					continue;

				present_faces.insert(idx);
				visible_faces.push_back(faces[idx]);
			}
		}
	} else {
		visible_faces = faces;
	}

	indices.clear();

	for (auto& face : visible_faces) {
		for (i16 i = 1, j = 2; j < face.n_surf_edges; i++, j++) {
			indices.push_back(face.first_surf_edge_idx);
			indices.push_back(face.first_surf_edge_idx+i);
			indices.push_back(face.first_surf_edge_idx+j);
		}
//		indices.push_back(get_index_from_surfedge(face.first_surf_edge_idx));
	}

	index_buffer->upload(indices);
}

int BSP::get_index_from_surfedge(int surfedge) {
	int surf = surfedges[surfedge];
	if(surf >= 0) {
		return edges[surf].vertex_indices[0];
	}
	else {
		return edges[-surf].vertex_indices[1];
	}
}

int BSP::determine_leaf(glm::vec3 cam_pos) {
	/* use SDF of planes to determine relative position with respect to partitioning planes */
	int idx = 0;
	/* positive values are node indices, negative values are leaf indices */
	while (idx >= 0) {
		const auto& plane = planes[nodes[idx].plane];
		const auto dist = glm::dot(plane.norm, cam_pos) - plane.dist;

		if (dist >= 0)
			idx = nodes[idx].children[0];
		else
			idx = nodes[idx].children[1];
	}

	return -idx - 1;
}


bool BSP::determine_visibility(const Leaf& cam_leaf, const Leaf& leaf, const std::array<glm::vec4, 6>& frustum, const glm::vec3 box_verts[8]) {
	/* perform fustrum culling */
	return box_in_frustum(frustum, box_verts);
}

/* changes handedness by swapping z and y */
template<typename T>
static inline void change_swizzle(T& v) {
	auto tmp = v.y;
	v.y = v.z;
	v.z = tmp;
}

static std::vector<std::map<std::string, std::string>> load_entities(const std::string& in) {
	/* TODO */
	return {
		{{"test", "this"}},
	};
}

static std::vector<MipTexture> load_mip_textures(const u8* data, u32 offset) {
	const TextureLump* lump = reinterpret_cast<const TextureLump*>(data + offset);
	std::vector<MipTexture> ret;
	ret.resize(lump->n_mip_textures);

	for(size_t i = 0; i < ret.size(); i++) {
		ret[i] = *reinterpret_cast<const MipTexture*>(data + lump->offsets[i]);
	}

	return ret;
}

BSP::BSP(vk::PhysicalDevice phys_dev, vk::Device dev, const std::string& fname) : dev(dev), filename(fname) {
	file_data = file::slurpb(fname);
	Log::debug("BSP file size: %zu\n", file_data.size());
	header = reinterpret_cast<Header*>(file_data.data());

	Log::info("Loading BSP: %s\n", fname.c_str());

	if(header->version != 30) {
		Log::error("BSP file not expected version (Half Life has version 30)!\n");
	}

	Log::debug("Loading entities\n");
	std::string entities_buff;
	copy_data(file_data.data(), entities_buff, header->entities);
	entities = load_entities(entities_buff);

	Log::debug("Loading planes\n");
	copy_data(file_data.data(), planes, header->planes);
	/* change swizzle */
	for (auto& plane : planes) {
		change_swizzle(plane.norm);
	}

	Log::debug("Loading textures\n");
	textures = load_mip_textures(file_data.data(), header->textures.offset);

	Log::debug("Loading vertices\n");
	copy_data(file_data.data(), vertices, header->vertices);
	for (auto& vertex : vertices) {
		change_swizzle(vertex.pos);
	}

	Log::debug("Loading nodes\n");
	copy_data(file_data.data(), nodes, header->nodes);
	for (auto& node : nodes) {
		change_swizzle(node.bb_mins);
		change_swizzle(node.bb_maxes);
	}

	Log::debug("Loading texinfo\n");
	copy_data(file_data.data(), tex_infos, header->texinfo);

	Log::debug("Loading faces\n");
	copy_data(file_data.data(), faces, header->faces);

	Log::debug("Loading lightmap\n");
	lightmap.lights = reinterpret_cast<rgb*>(file_data.data()+header->lighting.offset);

	Log::debug("Loading clip nodes\n");
	copy_data(file_data.data(), clip_nodes, header->clip_nodes);
	
	Log::debug("Loading leaves\n");
	copy_data(file_data.data(), leaves, header->leaves);
	for (auto& leaf : leaves) {
		change_swizzle(leaf.bb_mins);
		change_swizzle(leaf.bb_maxes);
	}

	Log::debug("Loading mark surfaces\n");
	copy_data(file_data.data(), mark_surfaces, header->mark_surfaces);

	Log::debug("Loading edges\n");
	copy_data(file_data.data(), edges, header->edges);

	Log::debug("Loading surfedges\n");
	copy_data(file_data.data(), surfedges, header->surf_edges);
	vertices_prime.reserve(surfedges.size());
	/* use this to build our vertices_prime, idea thanks to gzalo's HalfMapper */
	for(const auto& s : surfedges) {
		vertices_prime.push_back(vertices[edges[s > 0? s : -s].vertex_indices[s<=0]]);
	}

	Log::debug("Loading models\n");
	copy_data(file_data.data(), models, header->models);
	for (auto& model : models) {
		change_swizzle(model.bb_mins);
		change_swizzle(model.bb_maxes);
	}
	
	Log::debug("Creating vertex buffer of size %zu\n", vertices.size());
	vertex_buffer = std::make_unique<GeneralVertexBuffer<Vertex>>(phys_dev, dev, vertices_prime.size());
	vertex_buffer->upload(vertices_prime);

	Log::debug("Creating index buffer\n");
	/* set limit at 256Mi indices */
	index_buffer = std::make_unique<Buffer>(phys_dev, dev, 100000 * sizeof(u32),
		vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
	);
}