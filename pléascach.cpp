#include <GLFW/glfw3.h>

#include <window/window.hpp>

#include <iostream>

int main() {
	try {
		Window win("Test", 256, 256);

		win.run();
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}
