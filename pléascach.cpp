#include <window/window.hpp>
#include <input/input.hpp>
#include <renderer/renderer.hpp>

#include <util/log.hpp>


#include <iostream>

int main() {
	try {
		Window win("Test", 256, 512);
		auto in = win.getInput();
		Renderer ren(win);

		while (!in->shouldClose()) {
			in->poll();

			while (in->queue.size()) {
				/* take event from front of queue, then process it */
				auto event = in->queue.front();
				in->queue.pop();
				switch (event.tag) {
					case InputEvent::Tag::RESIZE:
						Log::info("Event Processed: Resized to %dx%d\n", event.resize.width, event.resize.height);
						win.setDimensions(event.resize.width, event.resize.width);
						/* no need to have a resize() function in the renderer, b/c swapchain images will be
						 * automatically marked out-of-date, and recreation will be triggered in our code 
						 */
					break;
					case InputEvent::Tag::KEY:
						Log::info("Event Processed: Button 0x%x %d\n", event.key.key, event.key.state);
						win.close();
					break;
				}
			}
		}

	} catch (const std::string& e) {
		std::cerr << "Exception: " << e << std::endl;
	}
}
