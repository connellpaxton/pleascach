#include <GLFW/glfw3.h>

#include <window/window.hpp>

#include <iostream>

int main() {
	try {
		Window win("Test", 256, 256);

		win.run();
	} catch (std::string& e) {
		std::cerr << "Exception: " << e << std::endl;
	}
}
