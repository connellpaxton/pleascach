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
	//	static const char* action_strings[] = { "RELEASE", "PRESS", "REPEAT" };
	//	Log::info("Event Received:Key:\"%s\" %s\n", glfwGetKeyName(key, scancode), action_strings[action]);
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
}

void Input::setCursor(bool enabled) {
	glfwSetInputMode(in, GLFW_CURSOR, enabled? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void Input::handleMovementKeys(Renderer& ren) {
	if (ImGui::GetIO().WantCaptureKeyboard)
		return;

	glm::vec3 forward;
	if (ren.flycam)
		forward = glm::normalize(glm::vec3(glm::sin(ren.cam.theta)*glm::cos(ren.cam.phi), glm::cos(ren.cam.theta), glm::sin(ren.cam.theta)*glm::sin(ren.cam.phi)));
	else {
		forward = glm::normalize(glm::vec3(glm::cos(ren.cam.phi), 0.0, glm::sin(ren.cam.phi)));
	}
	const auto right = glm::normalize(glm::cross(forward, glm::vec3(0.0, 1.0, 0.0)));
	auto speed = glfwGetKey(in, GLFW_KEY_LEFT_SHIFT)? 2.0f : 1.0f;
	speed *= ren.speed;

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
		ren.cam.pos += forward * 1.0f * speed;
	}

	if (glfwGetKey(in, GLFW_KEY_S)) {
		ren.cam.pos += forward * -1.0f * speed;
	}

	/* move "left" or "right" */
	if (glfwGetKey(in, GLFW_KEY_A)) {
		ren.cam.pos -= right * 1.0f * speed;
	}

	if (glfwGetKey(in, GLFW_KEY_D)) {
		ren.cam.pos += right * 1.0f * speed;
	}

	if(glfwGetKey(in, GLFW_KEY_SPACE)) {
		ren.cam.pos.y += 1.0 * speed;
	}

	if(glfwGetKey(in, GLFW_KEY_LEFT_CONTROL)) {
		ren.cam.pos.y -= 1.0 * speed;
	}

	ren.cam.theta = glm::clamp(ren.cam.theta, 0.01f, glm::pi<float>() - 0.01f);
	ren.cam.phi = glm::mod(ren.cam.phi, glm::two_pi<float>());
}


void Input::handleCursorMovement(Renderer& ren, double x, double y) {
	int rel_mouse_x = static_cast<int>(x) - last_mouse.x;
	int rel_mouse_y = static_cast<int>(y) - last_mouse.y;

	auto& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		return;

	if (ren.in_menu) {
		io.AddMousePosEvent(x, y);
		return;
	}

	ren.cam.phi += rel_mouse_x / 100.0;
	ren.cam.theta += rel_mouse_y / 100.0;

	last_mouse = glm::vec2(x,y);

	ren.cam.theta = glm::clamp(ren.cam.theta, 0.01f, glm::pi<float>() - 0.01f);
	ren.cam.phi = glm::mod(ren.cam.phi, glm::two_pi<float>());
}