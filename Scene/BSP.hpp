#pragma once

#include <util/int.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <string>

/* contains loading functions for Quake III-style BSPs */
namespace Q3BSP {
	struct Lump {
		u32 offset;
		u32 len;
	};
	using rgb = glm::u8vec3;
	using rgba = glm::u8vec4;

	/* "IBSP" */
	const uint32_t BSP_MAGIC = 0x50534249U;
	struct Header {
		u32 magic;
		u32 version;

		union {
			Lump lumps[17];
			struct {
				Lump	entities,
						textures,
						planes,
						nodes,
						leafs,
						leaf_faces,
						leaf_brushes,
						models,
						brushes,
						brush_sides,
						vertices,
						mesh_vertices,
						effects,
						faces,
						lightmaps,
						lightvols,
						vis_info;
			};
		};
	};

	struct Texture {
		char name[64];
		/* values of unknown meaning - TODO: check darkplaces or some other 3rd party loader */
		i32 flags;
		i32 contents;
	};

	struct Plane {
		glm::vec3 norm;
		float dist;
	};

	struct Node {
		u32 plane;
		/* negative numbers are leaf indices */
		i32 children[2];

		/* bounding box coords (integer) */
		glm::ivec3 bb_mins;
		glm::ivec3 bb_maxes;
	};

	struct Leaf {
		i32 cluster_idx;
		u32 area;
		glm::ivec3 bb_mins;
		glm::ivec3 bb_maxes;
		i32 first_leaf_face_idx;
		u32 n_leaf_faces;
		i32 first_leaf_brush_idx;
		u32 n_leaf_brushes;
	};


	struct LeafFaces {
		/* list of face indices (one list per leaf) */
		i32 face_idx;
	};

	struct LeafBrush {
		/* list of brush indices (one list leaf) */
		i32 brush_idx;
	};

	struct Model {
		glm::vec3 bb_mins;
		glm::vec3 bb_maxes;
		i32 first_face_idx;
		u32 n_faces;
		i32 first_brush_idx;
		u32 n_brushes;
	};

	struct Brush {
		i32 first_brushside_idx;
		u32 n_brushsides;
		i32 texture_idx;
	};

	struct BrushSide {
		i32 plane_idx;
		i32 texture_idx;
	};
	
	struct Vertex {
		glm::vec3 position;
		glm::vec2 tex_coords;
		glm::vec2 lightmap_coords;
		glm::vec3 normal;
		glm::u8vec4 color;
	};

	struct MeshVertex {
		i32 idx;
	};

	struct Effect {
		char name[64];
		i32 brush_idx;
		/* almost always 5 for some reason */
		i32 unknown;
	};

	struct Face {
		i32 texture_idx;
		/* -1 if no effect */
		i32 effect_idx;
		enum FaceType {
			ePOLYGON = 1,
			ePATCH = 2,
			eMESH = 3,
			eBILLBOARD = 4,
		} type;
		i32 first_vertex_idx;
		u32 n_vertices;
		i32 first_mesh_vertex_idx;
		u32 n_mesh_vertices;
		i32 lightmap_idx;
		glm::vec2 lightmap_start;
		glm::vec2 lightmap_end;

		glm::vec3 lightmap_origin;
		glm::vec3 lightmap_unit_vectors[2];
		glm::vec3 norm;
		glm::ivec2 patch_dimensions;
	};

	struct Lightmap {
		u8 map[128][128][3];
	};

	struct Lightvol {
		rgb ambient;
		rgb directional;
		/* spherical coordinates */
		glm::u8vec2 direction;
	};

	struct VisibilityInfo {
		u32 sz_vectors;
		std::vector<u8> vectors;
	};

	struct BSP {
		Header* header;
		BSP(const std::string& fname);
		std::string filename;
		std::vector<u8> file_data;
		std::string entities;
		std::vector<Texture> textures;
		std::vector<Plane> planes;
		std::vector<Node> nodes;
		std::vector<Leaf> leafs;
		std::vector<LeafBrush> leaf_brushes;
		std::vector<LeafFaces> leaf_faces;
		std::vector<Model> models;
		std::vector<Brush> brushes;
		std::vector<BrushSide> brush_sides;
		std::vector<Vertex> vertices;
		std::vector<MeshVertex> mesh_vertices;
		std::vector<Effect> effects;
		std::vector<Face> faces;
		std::vector<Lightmap> lightmaps;
		std::vector<Lightvol> lightvols;
		VisibilityInfo vis_info;
	};
}