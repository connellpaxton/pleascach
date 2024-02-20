#include <Scene/BSP.hpp>

#include <Renderer/Pipeline.hpp>

#include <util/geo.hpp>
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
	std::memcpy(dst.data(), ((u8*)file_data) + lump.offset, lump.len);
}

void BSP::load_indices(const glm::vec3& cam_pos, bool visibility_test, const glm::mat4& view) {
	std::set<int> present_faces;
	std::vector<Face> visible_faces;
	if (visibility_test) {
		auto leaf_idx = determine_leaf(cam_pos);

		auto fr_planes = frustum(view);

		if (leaf_idx == last_leaf)
			index_buffer->upload(indices);

		last_leaf = leaf_idx;
		auto& cam_leaf = leafs[leaf_idx];


		std::vector<Leaf> visible_leafs;
		for (auto& leaf : leafs) {

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
			for (size_t i = 0; i < leaf.n_leaf_faces; i++) {
				auto idx = leaf_faces[leaf.first_leaf_face_idx + i].face_idx;
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

	assert(indices.size() % 3 == 0);

	index_buffer->upload(indices);
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
	int vis = cam_leaf.cluster_idx, cluster = leaf.cluster_idx;
	if (vis_info.vectors.size() == 0 || vis < 0)
		return true;

	int i = (vis * vis_info.sz_vectors) + (cluster >> 3);
	u8 set = vis_info.vectors[i];

	if (!(set & (1 << (cluster & 0x7))))
		return false;

	/* perform fustrum culling */
	return box_in_frustum(frustum, box_verts);
}

/* changes handedness by swapping z and y */
static inline void change_swizzle(glm::vec3& v) {
	auto tmp = v.y;
	v.y = v.z;
	v.z = tmp;
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
	/* change swizzle */
	for (auto& plane : planes) {
		change_swizzle(plane.norm);
	}
	copy_data(file_data.data(), nodes, header->nodes);
	copy_data(file_data.data(), leafs, header->leafs);
	copy_data(file_data.data(), leaf_faces, header->leaf_faces);
	copy_data(file_data.data(), leaf_brushes, header->leaf_brushes);
	copy_data(file_data.data(), models, header->models);
	copy_data(file_data.data(), brushes, header->brushes);
	copy_data(file_data.data(), brush_sides, header->brush_sides);
	copy_data(file_data.data(), vertices, header->vertices);
	/* correct for handedness */
	for (auto& vertex : vertices) {
		change_swizzle(vertex.pos);
		change_swizzle(vertex.norm);
	}

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
	index_buffer = std::make_unique<Buffer>(phys_dev, dev, 100000 * sizeof(u32),
		vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
	);
}