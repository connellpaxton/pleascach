#pragma once

#include <renderer/Renderer.hpp>

#include <GLFW/glfw3.h>
#include <string>

using namespace std::string_literals;

struct Window {
	std::string name;
	unsigned width, height;
	GLFWwindow* win;
	bool inited = false;
	std::unique_ptr<Renderer> ren;

	Window(const std::string& name, unsigned width, unsigned height);
	void run();
	void wait_minimize();
	~Window();
};