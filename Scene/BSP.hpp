#pragma once

#include <util/int.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <string>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp> 

#include <Renderer/VertexBuffer.hpp>
#include <Renderer/Pipeline.hpp>


#define MAX_TEXTURE_NAME 16
#define MIP_LEVELS 4

#define MAX_MAP_HULLS 4
/* contains loading functions for Half Life BSPs */
namespace HLBSP {

	struct Lump {
		u32 offset;
		u32 len;
	};
	using rgb = glm::u8vec3;
	using rgba = glm::u8vec4;

	using vec3 = glm::vec3;
	using ivec3 = glm::vec<3, i16>;

	struct Header {
		u32 version;

		union {
			Lump lumps[15];
			struct {
				Lump	entities,
						planes,
						textures,
						vertices,
						visibility,
						nodes,
						texinfo,
						faces,
						lighting,
						clip_nodes,
						leaves,
						mark_surfaces,
						edges,
						surf_edges,
						models;
			};
		};
	};

	struct Plane {
		vec3 norm;
		float dist;
		/* exists for certain optimizations (swaped y and z) */
		enum PlaneType {
			eX,
			eZ,
			eY,
			eAnyX,
			eAnyZ,
			eAnyY,
		} type;
	};

	struct TextureLump {
		u32 n_mip_textures;
		i32 offsets[];
	};

	struct MipTexture {
		char name[MAX_TEXTURE_NAME];
		u32 width, height;
		/* is 0 if stored in WAD, otherwise, offset is from beginning of this struct */
		u32 mip_offsets[MIP_LEVELS];
	};

	struct Vertex {
		vec3 pos;

		static inline std::vector<vk::VertexInputAttributeDescription> attrs(uint32_t binding) {
			return std::vector<vk::VertexInputAttributeDescription> {
				{
					.location = 0,
					.binding = binding,
					.format = vk::Format::eR32G32B32Sfloat,
					.offset = offsetof(Vertex, pos),
				}
			};
		}
	};

	struct Vis {};

	struct Node {
		i32 plane;
		/* negative numbers are leaf indices */
		i16 children[2];

		/* bounding box coords (integer) */
		ivec3 bb_mins;
		ivec3 bb_maxes;

		i16 first_face_idx;
		i16 n_faces;
	};

	struct TexInfo {
		vec3 shift_s_dir;
		float shift_s;
		vec3 shift_t_dir;
		float shift_t;
		u32 mip_tex_idx;
		/* seems to always be 0 */
		u32 flags;
		
	};

	struct Face {
		u16 plane_idx;
		/* set if different normals orientation */
		u16 plane_side;
		u32 first_surf_edge_idx;
		i16 n_surf_edges;
		i16 tex_info_idx;
		u8 lighting_styles[4];
		u32 lightmap_offset;
	};

	struct Lightmap {
		rgb* lights;
	};

	struct ClipNode {
		i32 plane_idx;
		/* negative numbers are contents */
		i16 children[2];
	};

	struct Leaf {
		enum {
			eEmpty = -1,
			eSolid = -2,
			eWater = -3,
			eSlime = -4,
			eLava = -5,
			eSky = -6,
			eOrigin = -7,
			eClip = -8,
			eCurrent0 = -9,
			eCurrent90 = -10,
			eCurrent180 = -11,
			eCurrent270 = -12,
			eCurrentUp = -13,
			eCurrentDown = -14,
			eTranslucent = -15,
		} contents;
		/* if this is -1, no VIS data */
		i32 vis_offset;
		ivec3 bb_mins;
		ivec3 bb_maxes;
		u16 first_mark_surface_idx;
		u16 n_mark_surfaces;
		u8 ambient_sound_levels[4];
	};

	typedef u16 MarkSurface;

	struct Edge {
		u16 vertex_indices[2];
	};

	typedef i32 Surfedge;

	struct Model {
		vec3 bb_mins;
		vec3 bb_maxes;
		vec3 origin;
		i32 head_node_indices[MAX_MAP_HULLS];
		i32 vis_leafs;
		i32 first_face_idx;
		i32 n_faces;
	};

	struct BSP {
		BSP(vk::PhysicalDevice phys_dev, vk::Device dev, const std::string& fname);
		void load_indices(const vec3& cam_pos, bool visibility_testing, const glm::mat4& view);
		int determine_leaf(vec3 cam_pos);
		bool determine_visibility(const Leaf& cam_leaf, const Leaf& leaf, const std::array<glm::vec4, 6>& frustum, const vec3 box_verts[8]);
		int get_index_from_surfedge(int surfedge);


		vk::Device dev;
		Header* header;
		std::string filename;
		std::vector<u8> file_data;

		std::vector<::std::map<::std::string, std::string>> entities;
		std::vector<Plane> planes;
		std::vector<MipTexture> textures;
		std::vector<Vertex> vertices;
		std::vector<Vertex> vertices_prime;
		/* skipping vis for now */
		std::vector<Node> nodes;
		std::vector<TexInfo> tex_infos;
		std::vector<Face> faces;
		Lightmap lightmap;
		std::vector<ClipNode> clip_nodes;
		std::vector<Leaf> leaves;
		std::vector<MarkSurface> mark_surfaces;
		std::vector<Edge> edges;
		std::vector<Surfedge> surfedges;
		std::vector<Model> models;

		std::vector<u32> indices;
		std::unique_ptr<Buffer> index_buffer;
		std::unique_ptr<GeneralVertexBuffer<Vertex>> vertex_buffer;
		std::unique_ptr<GraphicsPipeline> pipeline;
		/* to eliminate needless re-loading*/
		int last_leaf = -0x1337;

		~BSP() {
			vertex_buffer.reset();
			pipeline.reset();
		}
	};
}