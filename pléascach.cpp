#include <Window/Window.hpp>
#include <Input/Input.hpp>
#include <Renderer/Renderer.hpp>

#include <util/log.hpp>
#include <util/Timer.hpp>

#include <iostream>

#include <GLFW/glfw3.h>


int main(int argc, char* argv[]) {
	try {
		Window win(argv[0], 256, 512);

		auto mon = glfwGetPrimaryMonitor();
		auto v = glfwGetVideoMode(mon);
		glfwSetWindowMonitor(win.win, mon, 0, 0, v->width, v->height, v->refreshRate);

		auto in = win.getInput();
		Renderer ren(win);
		in->setCursor(false);

		bool should_close = false;

		while (!in->shouldClose() || ren.should_close) {
			Timer frame_timer;
			in->poll();
			in->handleMovementKeys(ren);

			while (in->queue.size()) {
				/* take event from front of queue, then process it */
				const InputEvent event = in->queue.front();
				in->queue.pop();
				switch (event.tag) {
					case InputEvent::Tag::eRESIZE:
						Log::info("Event Processed: Resized to %dx%d\n", event.resize.width, event.resize.height);
						/* no need to have a resize() function in the renderer, b/c swapchain images will be
						 * automatically marked out-of-date, and recreation will be triggered in our code 
						 */
						/* but still block while waiting for window to be opened again */
						if (event.resize.height == 0 || event.resize.width == 0) {
							int h = event.resize.height;
							int w = event.resize.width;
							do {
								glfwPollEvents();
								glfwGetWindowSize(win.win, &w, &h);
							} while (h * w == 0);
						}
							
					break;
					case InputEvent::Tag::eEXIT:
						win.close();
					break;
					case InputEvent::Tag::eCURSOR:
						in->handleCursorMovement(ren, event.pos.x, event.pos.y);
					break;
					case InputEvent::Tag::eBUTTON:
					break;
					case InputEvent::Tag::eKEY:
						if (event.key.key == GLFW_KEY_ESCAPE && event.key.state == GLFW_PRESS) {
							ren.in_menu = !ren.in_menu;
							in->setCursor(ren.in_menu);
						} else if (event.key.key == GLFW_KEY_Q && event.key.state == GLFW_PRESS) {
							if (!ren.in_menu) {
								ren.should_close = true;
								goto quit;
							}
						}
					break;
				}
			}

			ren.draw();
			ren.present();
			ren.frametime = frame_timer.read();
			ren.fps = 1000.0f / ren.frametime;

			while (frame_timer.read() < 1000.0 / ren.max_fps)
				;
			
			ren.frametime = frame_timer.read();
		}

	} catch (const std::string& e) {
		std::cerr << "Exception: " << e << std::endl;
	}

quit:
	Log::info("Quitting");
}
