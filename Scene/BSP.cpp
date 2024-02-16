#include <Scene/BSP.hpp>

#include <util/file.hpp>
#include <util/log.hpp>

using namespace Q3BSP;

static inline void copy_data(void* file_data, std::string& dst, Lump& lump) {
	dst.resize(lump.len);
	std::memcpy(dst.data(), (u8*)file_data + (size_t)lump.offset, lump.len);
}

template<typename T>
static inline void copy_data(void* file_data, std::vector<T>& dst, Lump& lump) {
	dst.resize(lump.len / sizeof(T));
	//Log::debug("%p %p\n", dst.data(), (u8*)file_data + lump.offset);
	std::memcpy(dst.data(), (u8*)file_data + (size_t)lump.offset, lump.len);
}

BSP::BSP(const std::string& fname) : filename(fname) {
	file_data = file::slurpb(fname);
	Log::debug("File size: %zu\n", file_data.size());
	header = reinterpret_cast<Header*>(file_data.data());

	Log::info("Loading BSP: %s\n", fname.c_str());

	if(header->magic != BSP_MAGIC) {
		Log::error("BSP file missing magic!\n");
	}

	size_t i = 0;
	for (auto& lump : header->lumps) {
		i++;
		Log::debug("%i: Offset: %u | Length: %u\n", i, lump.offset, lump.len);
		Log::debug("\tPointer: 0x%p\n", (u8*)file_data.data() + (size_t)lump.offset);
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
	
	vis_info.sz_vectors = *reinterpret_cast<u32*>(file_data.data() + header->vis_info.offset);
	vis_info.vectors.resize(header->vis_info.len);
	std::memcpy(vis_info.vectors.data(), file_data.data() + header->vis_info.offset + sizeof(u32), header->vis_info.len);

	vis_info = *reinterpret_cast<VisibilityInfo*>(file_data.data() + header->vis_info.offset);
}