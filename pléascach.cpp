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

		while (!in->shouldClose()) {
			Timer frame_timer;
			in->poll();
			in->handleMovementKeys(ren);

			while (in->queue.size()) {
				/* take event from front of queue, then process it */
				const auto event = in->queue.front();
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
						if (event.key.key == GLFW_KEY_Q) {
							return 0;
						} else if (event.key.key == GLFW_KEY_ESCAPE && event.key.state == GLFW_PRESS) {
							ren.capture_mouse = !ren.capture_mouse;
							in->setCursor(!ren.capture_mouse);
						} else if (event.key.key == GLFW_KEY_R && event.key.state == GLFW_PRESS) {
							ren.time = 0;
						} else if (event.key.key == GLFW_KEY_C && event.key.state == GLFW_PRESS) {
							ren.flycam = !ren.flycam;
						} else if (event.key.key == GLFW_KEY_P && event.key.state == GLFW_PRESS) {
							ren.running = !ren.running;
						} else if (event.key.key == GLFW_KEY_T && event.key.state == GLFW_PRESS) {
							ren.speed *= 10.0;
						} else if (event.key.key == GLFW_KEY_Y && event.key.state == GLFW_PRESS) {
							ren.speed /= 10.0;
						}
					break;
				}
			}

			ren.draw();
			ren.present();
			const auto t = frame_timer.read();
			ren.ui->info.fps = 1000.0f / t;
			ren.time += t / 1000.0 * ren.speed * static_cast<float>(ren.running);

			/*while (frame_timer.read() < 16.60)
				;*/
		}

	} catch (const std::string& e) {
		std::cerr << "Exception: " << e << std::endl;
	}  
}
