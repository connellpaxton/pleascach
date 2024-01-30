#pragma once

#ifndef INPUT_PTR
#define INPUT_PTR void*
#endif

#include <queue>
#include <util/int.hpp>

#include <glm/glm.hpp>

enum InputModifierBit {
	eNONE  = 0x00,
	eSHIFT = 0x01,
	eCNTRL = 0x02,
};

constexpr InputModifierBit operator & (const InputModifierBit a, const InputModifierBit b) {
	return static_cast<InputModifierBit>(static_cast<u32>(a) & static_cast<u32>(b));
}

constexpr InputModifierBit operator | (const InputModifierBit a, const InputModifierBit b) {
	return static_cast<InputModifierBit>(static_cast<u32>(a) & static_cast<u32>(b));
}

constexpr InputModifierBit& operator |=(InputModifierBit& a, const InputModifierBit b) {
	a = static_cast<InputModifierBit>(static_cast<u32>(a) | static_cast<u32>(b));
	return a;
}

constexpr u32 operator ~ (InputModifierBit a) {
	return ~static_cast<u32>(a);
}

struct InputEvent {
	enum Tag {
		eEXIT,
		eRESIZE,
		eCURSOR,
		eKEY,
		eBUTTON,
	} tag;

	union {
		struct {
			int width, height;
		} resize;
		struct {
			double x, y;
		} pos;
		struct {
			int key;
			int state;
			InputModifierBit mods;
		} key;
	};
};

struct Renderer;

struct Input {
	Input(INPUT_PTR in);

	INPUT_PTR in;

	glm::vec2 last_mouse = glm::vec2(0.0f, 0.0f);

	void poll();

	std::queue<InputEvent> queue;

	void handleMovementKeys(Renderer& ren);
	void handleCursorMovement(Renderer& ren, double x, double y);

	bool shouldClose();
};