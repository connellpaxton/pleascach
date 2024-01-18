#pragma once

#ifndef INPUT_PTR
#define INPUT_PTR void*
#endif

#include <queue>
#include <util/int.hpp>

enum InputModifierBit {
	NONE  = 0x00,
	SHIFT = 0x01,
	CNTRL = 0x02,
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
		RESIZE,
		CURSOR,
		KEY,
		BUTTON,
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

struct Input {
	Input(INPUT_PTR in);

	INPUT_PTR in;

	void poll();

	std::queue<InputEvent> queue;

	bool shouldClose();
};