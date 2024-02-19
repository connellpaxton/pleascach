#include <Scene/BSP.hpp>

#include <Renderer/Pipeline.hpp>

#include <util/file.hpp>
#include <util/log.hpp>

#include <set>

#include <cstring>

using namespace Q3BSP;

static inline void copy_data(void* file_data, std::string& dst, Lump& lump) {
	dst.resize(lump.len);
	std::memcpy(dst.data(), (u8*)file_data + (size_t)lump.offset, lump.len);
}

template<typename T>
static inline void copy_data(void* file_data, std::vector<T>& dst, Lump& lump) {
	dst.resize(lump.len / sizeof(T));
	//Log::debug("%p %p\n", dst.data(), (u8*)file_data + lump.offset);
	std::memcpy(dst.data(), ((u8*)file_data) + lump.offset, lump.len);
}

void BSP::load_indices(const glm::vec3& cam_pos) {
	std::set<int> present_faces;
	std::vector<Face> visible_faces;
	auto leaf_idx = determine_leaf(cam_pos);
	if (leaf_idx == last_leaf)
		return;

	last_leaf = leaf_idx;
	auto& cam_leaf = leafs[leaf_idx];


	std::vector<Leaf> visible_leafs;
	for (auto& leaf : leafs) {
		if (determine_visibility(cam_leaf.cluster_idx, leaf.cluster_idx))
			visible_leafs.push_back(leaf);
	}

	Log::debug("%zu visible leafs.\n", visible_leafs.size());

	for (const auto& leaf : visible_leafs) {
//		Log::debug("Faces: %zu vs %zu\n", leaf.first_leaf_face_idx + leaf.n_leaf_faces, faces.size());
		for (size_t i = 0; i < leaf.n_leaf_faces; i++) {
			auto idx = leaf_faces[leaf.first_leaf_face_idx + i].face_idx;
			if (present_faces.contains(idx))
				continue;

			present_faces.insert(idx);
//			Log::debug("Face idx: %zu (v.s. %zu)\n", idx, faces.size());
			visible_faces.push_back(faces[idx]);
		}
	}
	Log::debug("%zu visible faces.\n", visible_leafs.size());

	for (auto& face : visible_faces) {
		switch (face.type) {
			case Face::ePATCH:
			break;
			case Face::ePOLYGON:
			case Face::eMESH:
				for (size_t i = 0; i < face.n_mesh_vertices; i++)
					indices.push_back(face.first_vertex_idx + mesh_vertices[face.first_mesh_vertex_idx+i].idx);
			break;
		}
	}

	index_buffer->upload(indices);
}

int BSP::determine_leaf(glm::vec3 cam_pos) {
	/* camera coordinate transformation */
	float tmp = cam_pos.y;
	cam_pos.y = cam_pos.z;
	cam_pos.z = tmp;

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


bool BSP::determine_visibility(int vis, int cluster) {
	if (vis_info.vectors.size() == 0 || vis < 0)
		return true;

	int i = (vis * vis_info.sz_vectors) + (cluster >> 3);
	u8 set = vis_info.vectors[i];

	return !!(set & (1 << (cluster & 0x7)));
}

BSP::BSP(vk::PhysicalDevice phys_dev, vk::Device dev, const std::string& fname) : dev(dev), filename(fname) {
	file_data = file::slurpb(fname);
	Log::debug("File size: %zu\n", file_data.size());
	header = reinterpret_cast<Header*>(file_data.data());

	Log::info("Loading BSP: %s\n", fname.c_str());

	if(header->magic != BSP_MAGIC) {
		Log::error("BSP file missing magic!\n");
	}

	copy_data(file_data.data(), entities, header->entities);
	copy_data(file_data.data(), textures, header->textures);
	copy_data(file_data.data(), planes, header->planes);
	copy_data(file_data.data(), nodes, header->nodes);
	copy_data(file_data.data(), leafs, header->leafs);
	copy_data(file_data.data(), leaf_faces, header->leaf_faces);
	copy_data(file_data.data(), leaf_brushes, header->leaf_brushes);
	copy_data(file_data.data(), models, header->models);
	copy_data(file_data.data(), brushes, header->brushes);
	copy_data(file_data.data(), brush_sides, header->brush_sides);
	copy_data(file_data.data(), vertices, header->vertices);
	copy_data(file_data.data(), mesh_vertices, header->mesh_vertices);
	copy_data(file_data.data(), effects, header->effects);
	copy_data(file_data.data(), faces, header->faces);
	copy_data(file_data.data(), lightmaps, header->lightmaps);
	copy_data(file_data.data(), lightvols, header->lightvols);

	vis_info.sz_vectors = reinterpret_cast<u32*>(file_data.data() + header->vis_info.offset)[1];
	auto sz = header->vis_info.len;
	vis_info.vectors.resize(sz);
	std::memcpy(vis_info.vectors.data(), file_data.data() + header->vis_info.offset + 2*sizeof(u32), sz);

	vertex_buffer = std::make_unique<GeneralVertexBuffer<Vertex>>(phys_dev, dev, vertices.size());
	vertex_buffer->upload(vertices);

	/* set limit at 256Mi indices */
	index_buffer = std::make_unique<Buffer>(phys_dev, dev, 0x1000000 * sizeof(u32),
		vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
	);
}