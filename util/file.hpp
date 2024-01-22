#pragma once

#include <cstdint>
#include <fstream>
#include <sstream>

namespace file {
	static std::vector<uint8_t> slurpb(const std::string& fname) {
		std::ifstream in(fname, std::ifstream::binary | std::ifstream::in | std::ifstream::ate);
		auto sz = in.tellg();
		in.seekg(0, std::fstream::beg);
		std::vector<uint8_t> ret(sz);
		in.read(reinterpret_cast<char*>(ret.data()), sz);
		return ret;
	}

	static std::string slurp(const std::string& fname) {
		std::ifstream in(fname, std::ifstream::in);
		std::stringstream sstr;
		sstr << in.rdbuf();
		in.close();
		return sstr.str();
	}
}