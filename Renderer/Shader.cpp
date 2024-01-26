#include <Renderer/Shader.hpp>

#include <util/log.hpp>
#include <util/file.hpp>

using namespace std::string_literals;

Shader::Shader(vk::Device dev, const std::string& fname, vk::ShaderStageFlagBits stage) : dev(dev), fname(fname), stage(stage) {
	std::vector<uint8_t> src;
	try {
		src = file::slurpb(fname);
	} catch (std::exception& e) {
		Log::error("Failed to read file " + fname + ": "+ e.what() + "\n");
	}

	auto module_info = vk::ShaderModuleCreateInfo{
		.codeSize = src.size(),
		.pCode = reinterpret_cast<const uint32_t*>(src.data()),
	};

	if (!(module = dev.createShaderModule(module_info))) {
		Log::error(fname + ": failed to create shader\n");
	}

	Log::info("Successfully created shader "s + fname + "\n");
}

void Shader::cleanup() {
	dev.destroyShaderModule(module);
}

