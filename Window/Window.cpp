#include <window/window.hpp>

static std::string glfw_except(const std::string& msg) {
	const char* errmsg = NULL;
	glfwGetError(&errmsg);
	return msg + ": " + errmsg;
}

Window::Window(const std::string& name, unsigned width, unsigned height) : name(name), width(width), height(height) {
	if (!glfwInit()) {
		throw glfw_except("glfwInit()");
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	win = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

	if (!win) {
		throw glfw_except("glfwCreateWindow()");
	}

	ren = std::make_unique<Renderer>(this);

	inited = true;
}

void Window::run() {
	while (!glfwWindowShouldClose(win)) {
		glfwPollEvents();
	}
}

void Window::wait_minimize() {
	glfwGetFramebufferSize(win, reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height));
	while (!(width && height)) {
		glfwGetFramebufferSize(win, reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height));
		glfwWaitEvents();
	}
}

Window::~Window() {
	if(inited)
		glfwDestroyWindow(win);
	glfwTerminate();
}