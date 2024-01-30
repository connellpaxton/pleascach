﻿#include <Window/Window.hpp>
#include <Input/Input.hpp>
#include <Renderer/Renderer.hpp>

#include <util/log.hpp>
#include <util/Timer.hpp>

#include <iostream>

#include <GLFW/glfw3.h>


int main(int argc, char* argv[]) {
	try {
		Window win(argv[0], 256, 512);
		auto in = win.getInput();
		Renderer ren(win);

		while (!in->shouldClose()) {
			Timer frame_timer;
			in->poll();
			in->handleMovementKeys(ren);

			while (in->queue.size()) {
				/* take event from front of queue, then process it */
				const auto& event = in->queue.front();
				in->queue.pop();
				switch (event.tag) {
					case InputEvent::Tag::eRESIZE:
						Log::info("Event Processed: Resized to %dx%d\n", event.resize.width, event.resize.height);
						/* no need to have a resize() function in the renderer, b/c swapchain images will be
						 * automatically marked out-of-date, and recreation will be triggered in our code 
						 */
					break;
					case InputEvent::Tag::eEXIT:
						win.close();
					break;
					case InputEvent::Tag::eCURSOR:
					break;
					case InputEvent::Tag::eBUTTON:
					break;
				}
			}

			ren.draw();
			ren.present();
			const auto t = frame_timer.read();
			ren.ui->info.fps = 1000.0f / t;

			while (frame_timer.read() < 16.60)
				;
		}

	} catch (const std::string& e) {
		std::cerr << "Exception: " << e << std::endl;
	}  
}
