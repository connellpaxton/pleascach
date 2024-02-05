#include <Renderer/CommandBuffer.hpp>
#include <Scene/Terrain.hpp>

#include <util/int.hpp>


float Terrain::getHeight(int32_t x, int32_t y) {
	if (x < 0)
		x += 64;
	if (y < 0)
		y += 64;

	x %= 64;
	y %= 64;

	return heightmap_tex->image_data[(y * heightmap_tex->extent.height * heightmap_tex->extent.width / 64 + x * heightmap_tex->extent.width / 64) * 4];
}

Terrain::Terrain(vk::PhysicalDevice phys_dev, vk::Device dev, Texture& tex) : phys_dev(phys_dev), dev(dev) {
	/* tell Texture() not to free so we can apply our sobel filter */
	heightmap_tex = &tex;

	const auto patch_size = 64_u32;
	const auto uv_scale = 1.0f;
	const auto vertex_count = patch_size * patch_size;

	vertices.resize(vertex_count);

	for (size_t x = 0; x < patch_size; x++)
		for (size_t y = 0; y < patch_size; y++)
			vertices[x + y*patch_size] = (Vertex {
				.pos = glm::vec3(
					2.0f * x + 1.0f - patch_size,
					0.0f,
					2.0f * y + 1.0f - patch_size),
				.uv = glm::vec2(static_cast<float>(x)/patch_size, static_cast<float>(y) / patch_size) * uv_scale,
			});

	/* use sobel filters to get normal:
	*	X sobel: 
	*		+----+----+----+
	*		| +1 | +0 | -1 |
	*		+----+----+----+
	*		| +2 | +0 | -2 |
	*		+----+----+----+
	*		| +1 | +0 | -1 |
	*		+----+----+----+
	*	Y sobel: 
	*		+----+----+----+
	*		| +1 | +2 | +1 |
	*		+----+----+----+
	*		| +0 | +0 | +0 |
	*		+----+----+----+
	*		| -1 | -2 | -1 |
	*		+----+----+----+
	*/

	for(auto x = 0_i32; x < patch_size; x++)
		for (auto y = 0_i32; y < patch_size; y++) {
			float moores_heights[3][3] = {
				{ getHeight(x - 1, y - 1), getHeight(x - 1, y), getHeight(x - 1, y + 1) },
				{ getHeight(x + 0, y - 1), getHeight(x + 0, y), getHeight(x + 0, y + 1) },
				{ getHeight(x + 1, y - 1), getHeight(x + 1, y), getHeight(x + 1, y + 1) },
			};

			auto normal = glm::vec3(
				/* x gets X sobel filter */
				  moores_heights[0][0] + 2.0f * moores_heights[0][1] + moores_heights[0][2]
				- moores_heights[2][0] - 2.0f * moores_heights[2][1] - moores_heights[2][2],
				0.0,
				/* z gets Y sobel filter */
				  moores_heights[0][0] + 2.0f * moores_heights[1][0] + moores_heights[2][0]
				- moores_heights[0][2] - 2.0f * moores_heights[1][2] - moores_heights[2][2]
			);
			/* fill in missing component, first scalar scales bump */
			normal.y = 0.25 * glm::sqrt(1.0 -glm::dot(normal, normal));
			
			vertices[x + y * patch_size].norm = glm::normalize(normal * glm::vec3(2.0f, 1.0f, 2.0f));
		}

	vertex_buffer = std::make_unique<VertexBuffer>(phys_dev, dev, vertices.size());
	vertex_buffer->upload(vertices);

	/* index generation */
	const auto w = patch_size - 1;
	indices.resize(w * w * 4);

	for (auto x = 0_u32; x < w; x++)
		for (auto y = 0_u32; y < w; y++) {
			auto idx = (x + y * w) * 4;
			indices[idx] = x+y*patch_size;
			indices[idx+1] = indices[idx] + patch_size;
			indices[idx+2] = indices[idx+1] + 1;
			indices[idx + 3] = indices[idx] + 1;
		}

	index_buffer = std::make_unique<Buffer>(phys_dev, dev, sizeof(uint32_t)*indices.size(),
					vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eHostCoherent
					| vk::MemoryPropertyFlagBits::eHostVisible);

	index_buffer->upload(indices);
}

void Terrain::draw(CommandBuffer& cmd) {
}

Terrain::~Terrain() {
	index_buffer.reset();
	vertex_buffer.reset();
}