# Pléascach

## Short Term Changes
- Make index buffer device-local instead of host-coherent
	- Possibly restructure Buffer class to use templates to change
		constructor and functions to use staging buffers if needed.
## Constant Improvements
- Add more comments
## Long Term Improvements
- ~~Properly query surface to find supported formats for surfaces~~
- Fix all this cleanup vs destructor NONSENSE (inconsistency)
- Add pipeline caching
- Make more robust solution to window minimization (flushing out Input system should provide candidates)
- Command buffer per swapchain image
- Make smaller memory allocation object to pass around instead of directly passing physical device to everything