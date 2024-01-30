#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>


#define WINDOW_PTR GLFWwindow*
#define INPUT_PTR GLFWwindow*
#include <Window/Window.hpp>
#include <Input/Input.hpp>

#include <util/log.hpp>

static void win_err_callback(int code, const char* msg) {
	Log::error("GLFW: %i, \"%s\"", code, msg);
}

Window::Window(const std::string& title, u32 width, u32 height) : width(width), height(height) {
	glfwSetErrorCallback(win_err_callback);

	if (!glfwInit()) {
		Log::error("Failed to initialize GLFW\n");
		return;
	}

	if (!glfwVulkanSupported()) {
		Log::error("GLFW does not support vulkan\n");
		return;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	win = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);


	if (!win) {
		const char* msg = nullptr;
		glfwGetError(&msg);
		Log::error("Failed to create GLFW Window: %s\n", msg);
	}

	Log::info("Successfully created window with dimensions: %ux%u\n", getDimensions().width, getDimensions().height);
}

void Window::close() {
	glfwSetWindowShouldClose(win, GLFW_TRUE);
}

void Window::setDimensions(const u32 w, const u32 h) {
	glfwSetWindowSize(win, w, h);
}

void Window::getDimensions(u32& w, u32& h) {
	glfwGetFramebufferSize(win, reinterpret_cast<int*>(&w), reinterpret_cast<int*>(&h));
}

vk::Extent2D Window::getDimensions() {
	vk::Extent2D ret;
	getDimensions(ret.width, ret.height);
	return ret;
}

std::vector<const char*> Window::requiredExtensions() {
	u32 n = 0;
	auto arr = glfwGetRequiredInstanceExtensions(&n);
	std::vector<const char*> ret(n);
	std::memcpy(ret.data(), arr, sizeof(char*) * n);
	return ret;
}

vk::SurfaceKHR Window::getSurface(vk::Instance& inst) {
	VkSurfaceKHR ret;
	if (glfwCreateWindowSurface(inst, win, nullptr, &ret) != VK_SUCCESS) {
		return VK_NULL_HANDLE;
	}
	return ret;
}

std::unique_ptr<Input> Window::getInput() {
	return std::make_unique<Input>(win);
}

Window::~Window() {
	glfwDestroyWindow(win);
	/* if multiple windows are ever used, ensure this line is removed from destructor */
	glfwTerminate();
	Log::info("Terminated GLFW\n");

}
