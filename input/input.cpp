#include <GLFW/glfw3.h>
#define INPUT_PTR GLFWwindow*

#include <input/input.hpp>

#include <util/log.hpp>


bool Input::shouldClose() {
	return glfwWindowShouldClose(in);
}

void Input::poll() {
	glfwPollEvents();
}

Input::Input(INPUT_PTR in) : in(in) {
	glfwSetWindowUserPointer(in, this);

	glfwSetWindowSizeCallback(in, [](GLFWwindow* in, int width, int height) {
		Log::info("Event Received: Resize to %dx%d\n", width, height);
		Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(in));
		input->queue.push(InputEvent{
			.tag = InputEvent::Tag::RESIZE,
			.resize = {
				.width = width,
				.height = height,
			}
		});
	});

	glfwSetKeyCallback(in, [](GLFWwindow* in, int key, int scancode, int action, int mods) {
		int state = (action == GLFW_PRESS || action == GLFW_REPEAT);
		static const char* action_strings[] = { "RELEASE", "PRESS", "REPEAT" };
		Log::info("Event Received: Key: \"%s\" %s\n", glfwGetKeyName(key, scancode), action_strings[action]);
		Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(in));
		InputModifierBit i_mods = NONE;

		if(mods &= GLFW_MOD_SHIFT)
			i_mods |= SHIFT;
		if (mods &= GLFW_MOD_CONTROL)
			i_mods |= CNTRL;

		input->queue.push(InputEvent{
			.tag = InputEvent::Tag::KEY,
			.key = {
				.key = key,
				.state = state,
				.mods = i_mods,
			}
		});
	});

	glfwSetCursorPosCallback(in, [](GLFWwindow* in, double x, double y) {
		Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(in));

		input->queue.push(InputEvent{
			.tag = InputEvent::Tag::CURSOR,
			.pos = {
				.x = x,
				.y = y,
			}
		});
	});
}