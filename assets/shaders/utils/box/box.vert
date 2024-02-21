#version 460 core
/* simple passthrough */

layout (location = 0) in vec3 aMins;
layout (location = 1) in vec3 aMaxes;
layout (location = 2) in uint aId;

layout (location = 0) out vec3 mins;
layout (location = 1) out vec3 maxes;
layout (location = 2) out uint id;

void main() {
	mins = aMins;
	maxes = aMaxes;
	id = aId;
}