#include <Window/Window.hpp>
#include <Input/Input.hpp>
#include <Renderer/Renderer.hpp>

#include <util/log.hpp>
#include <util/Timer.hpp>

#include <iostream>

int main(int argc, char* argv[]) {
	try {
		Window win(argv[0], 256, 512);
		auto in = win.getInput();
		Renderer ren(win);

		while (!in->shouldClose()) {
			Timer frame_timer;
			in->poll();

			while (in->queue.size()) {
				/* take event from front of queue, then process it */
				auto event = in->queue.front();
				in->queue.pop();
				switch (event.tag) {
					case InputEvent::Tag::RESIZE:
						Log::info("Event Processed: Resized to %dx%d\n", event.resize.width, event.resize.height);
						/* no need to have a resize() function in the renderer, b/c swapchain images will be
						 * automatically marked out-of-date, and recreation will be triggered in our code 
						 */
					break;
					case InputEvent::Tag::KEY:
						Log::info("Event Processed: Button 0x%x %d\n", event.key.key, event.key.state);
					break;
					case InputEvent::Tag::EXIT:
						win.close();
					break;
					case InputEvent::Tag::CURSOR:
					case InputEvent::Tag::BUTTON:
					break;
				}
			}

			ren.draw();
			ren.present();
			Log::debug("Frame: %lf milliseconds (60fps ~ 16.67)\n", frame_timer.stop());
		}

	} catch (const std::string& e) {
		std::cerr << "Exception: " << e << std::endl;
	}  
}
