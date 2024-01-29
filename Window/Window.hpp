#pragma once

#include <util/int.hpp>

#include <string>
#include <vector>

#include <memory>

#include <Input/Input.hpp>

#ifndef WINDOW_PTR
#define WINDOW_PTR void*
#endif

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

/*
 * Window class - abstracts away implementation/platform-specific realities of window
 *
 *	Needs to be able to query and set window size, as well as report required instance extensions.
 *  Completely seperate from input.
 */

struct Window {
	/* Window title and dimensions. On failure, Window::win is nullptr, exception is thrown if DEBUG */
	Window(const std::string& title, u32 width, u32 height);
	~Window();

	void close();

	void setDimensions(const u32 x, const u32 y);
	void getDimensions(u32& w, u32& h);
	std::vector<const char*> requiredExtensions();
	vk::Extent2D getDimensions();
	vk::SurfaceKHR getSurface(vk::Instance& inst);
	std::unique_ptr<Input> getInput();

	WINDOW_PTR win = nullptr;

private:
	u32 width, height;
};

