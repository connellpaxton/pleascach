#pragma once

#include <Model/Model.hpp>

struct Object {
	Object(std::shared_ptr<Model> model);
	void draw();

	bool enable_coords = false;
	std::shared_ptr<Model> model;
};
