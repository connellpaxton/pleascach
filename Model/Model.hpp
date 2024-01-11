#pragma once

#include <string>

#include <Renderer/Renderer.hpp>

struct Model {
	Model(const std::string& fname, std::shared_ptr<Pipeline> pipeline);

	std::string fname;
	std::shared_ptr<Pipeline> pipeline;
};

