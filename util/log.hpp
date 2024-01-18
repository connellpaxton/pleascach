#pragma once

#include <string>
namespace Log {
	enum MessageLevelBit {
		ERROR = 0x01,
		INFO = 0x02,
	};

	static const MessageLevelBit log_mask = static_cast<MessageLevelBit>(~0);

	template<typename ...Args>
	static void print(MessageLevelBit level, const std::string& fmt, Args... args) {
		if (!(log_mask & level))
			return;

		/* appearently C++ doesn't have designated array indices :( */
		const char* level_txt = "[UNKNOWN] ";
		switch (level) {
		case ERROR:
			level_txt = "[ERROR] ";
			break;
		case INFO:
			level_txt = "[INFO] ";
			break;
		}
		std::fprintf(stderr, level_txt);
		std::fprintf(stderr, fmt.c_str(), args...);
	}

	template<typename ...Args>
	static void error(const std::string& fmt, Args... args) {
		print(MessageLevelBit::ERROR, fmt, args...);
#ifdef _DEBUG
		throw fmt;
#endif
	}

	template<typename ...Args>
	static void info(const std::string& fmt, Args... args) {
		print(MessageLevelBit::INFO, fmt, args...);
	}
}