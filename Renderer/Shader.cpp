#include <renderer/shader.hpp>

#include <util/log.hpp>

#include <fstream>
#include <sstream>

static std::string slurp(const std::string& fname) {
	std::ifstream in(fname, std::ifstream::binary | std::ifstream::in);
	std::stringstream sstr;
	sstr << in.rdbuf();
	in.close();
	return sstr.str();
}

Shader::Shader(vk::Device dev, const std::string& fname) : fname(fname) {
	std::string src;
	try {
		src = slurp(fname);
	} catch (std::exception& e) {
		Log::error("Failed to read file " + fname + ": "+ e.what() + "\n");
	}

	auto module_info = vk::ShaderModuleCreateInfo{
		.codeSize = src.size(),
		.pCode = reinterpret_cast<const uint32_t*>(src.c_str()),
	};

	if (!(module = dev.createShaderModule(module_info))) {
		Log::error(fname + ": failed to create shader\n");
	}
}

void Shader::cleanup(vk::Device dev) {
	dev.destroyShaderModule(module);
}
