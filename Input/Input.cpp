#include <GLFW/glfw3.h>
#define INPUT_PTR GLFWwindow*

#include <Input/Input.hpp>

#include <util/log.hpp>

#include <Renderer/Renderer.hpp>

#include <glm/glm.hpp>

#include <imgui/imgui_impl_glfw.h>

bool Input::shouldClose() {
	return glfwWindowShouldClose(in);
}

void Input::poll() {
	glfwPollEvents();
}

Input::Input(INPUT_PTR in) : in(in) {
	glfwSetWindowUserPointer(in, this);

	glfwSetWindowSizeCallback(in, [](GLFWwindow* in, int width, int height) {
		Log::info("Event Received:Resize to %dx%d\n", width, height);
		Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(in));
		input->queue.push(InputEvent{
			.tag = InputEvent::Tag::eRESIZE,
			.resize = {
				.width = width,
				.height = height,
			}
		});
	});

	glfwSetKeyCallback(in, [](GLFWwindow* in, int key, int scancode, int action, int mods) {
		int state = (action == GLFW_PRESS || action == GLFW_REPEAT);
		static const char* action_strings[] = { "RELEASE", "PRESS", "REPEAT" };
		Log::info("Event Received:Key:\"%s\" %s\n", glfwGetKeyName(key, scancode), action_strings[action]);
		Input* input = reinterpret_cast<Input*>(glfwGetWindowUserPointer(in));
		InputModifierBit i_mods = eNONE;

		if(mods &= GLFW_MOD_SHIFT)
			i_mods |= eSHIFT;
		if (mods &= GLFW_MOD_CONTROL)
			i_mods |= eCNTRL;

		input->queue.push(InputEvent{
			.tag = InputEvent::Tag::eKEY,
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
			.tag = InputEvent::Tag::eCURSOR,
			.pos = {
				.x = x,
				.y = y,
			}
		});
	});

	glfwSetInputMode(in, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::handleMovementKeys(Renderer& ren) {
	const auto forward = glm::vec3(glm::cos(ren.cam.phi), 0.0, glm::sin(ren.cam.phi));
	const auto right = glm::cross(forward, glm::vec3(0.0, 1.0, 0.0));
	const auto speed = glfwGetKey(in, GLFW_KEY_LEFT_SHIFT)? 2.0f : 1.0f;

	if(glfwGetKey(in, GLFW_KEY_UP)) {
		ren.cam.theta -= 0.01;
	}

	if(glfwGetKey(in, GLFW_KEY_UP)) {
		ren.cam.theta -= 0.01;
	}
	if(glfwGetKey(in, GLFW_KEY_DOWN)) {
		ren.cam.theta += 0.01;
	}
	
	if(glfwGetKey(in, GLFW_KEY_LEFT)) {
		ren.cam.phi -= 0.03;
	}
	
	if(glfwGetKey(in, GLFW_KEY_RIGHT)) {
		ren.cam.phi += 0.03;
	}
	/* move "forward" or "backward" */
	if (glfwGetKey(in, GLFW_KEY_W)) {
		ren.cam.pos += forward * 0.1f * speed;
	}

	if (glfwGetKey(in, GLFW_KEY_S)) {
		ren.cam.pos += forward * -0.1f * speed;
	}

	/* move "left" or "right" */
	if (glfwGetKey(in, GLFW_KEY_A)) {
		ren.cam.pos -= right * 0.1f * speed;
	}

	if (glfwGetKey(in, GLFW_KEY_D)) {
		ren.cam.pos += right * 0.1f * speed;
	}

	if(glfwGetKey(in, GLFW_KEY_SPACE)) {
		ren.cam.pos.y += 0.1;
	}

	if(glfwGetKey(in, GLFW_KEY_LEFT_CONTROL)) {
		ren.cam.pos.y -= 0.1;
	}

	ren.cam.theta = glm::clamp(ren.cam.theta, 0.01f, glm::pi<float>() - 0.01f);
	ren.cam.phi = glm::mod(ren.cam.phi, glm::two_pi<float>());
}


void Input::handleCursorMovement(Renderer& ren, double x, double y) {
	int rel_mouse_x = static_cast<int>(x) - last_mouse.x;
	int rel_mouse_y = static_cast<int>(y) - last_mouse.y;

	ren.cam.phi += rel_mouse_x / 100.0;
	ren.cam.theta += rel_mouse_y / 100.0;

	last_mouse = glm::vec2(x,y);

	ren.cam.theta = glm::clamp(ren.cam.theta, 0.01f, glm::pi<float>() - 0.01f);
	ren.cam.phi = glm::mod(ren.cam.phi, glm::two_pi<float>());
}