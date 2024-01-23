#pragma once

#include <chrono>

/* get the time in milliseconds */
struct Timer {
	std::chrono::time_point<std::chrono::steady_clock> start_time, end_time;
	bool running = false;


	Timer() {
		start();
	}

	inline void start() {
		start_time = std::chrono::steady_clock::now();
		running = true;
	}

	inline void reset() {
		start();
	}

	inline double read() {
		if (running) {
			auto end = std::chrono::steady_clock::now();
			return std::chrono::duration_cast<std::chrono::microseconds>(end - start_time).count() / 1000.0;
		}

		return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;
	}

	inline double stop() {
		end_time = std::chrono::steady_clock::now();
		running = false;
		return read();
	}
};