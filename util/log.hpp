#pragma once

#include <string>

namespace Log {
	enum MessageLevelBit {
		eERROR = 0x01,
		eINFO = 0x02,
		eDEBUG = 0x04,
	};

	inline MessageLevelBit operator | (MessageLevelBit a, MessageLevelBit b) {
		return static_cast<MessageLevelBit>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	static const MessageLevelBit log_mask = eERROR | eINFO | eDEBUG;

	template<typename ...Args>
	static void print(MessageLevelBit level, const std::string& fmt, Args... args) {
		if (!(log_mask & level))
			return;

		/* appearently C++ doesn't have designated array indices :( */
		const char* level_txt = "[UNKNOWN] ";
		switch (level) {
			case eERROR:
				level_txt = "[ERROR] ";
			break;
			case eINFO:
				level_txt = "[INFO] ";
			break;
			case eDEBUG:
				level_txt = "[DEBUG] ";
			break;
		}
		std::fprintf(stderr, level_txt);
		std::fprintf(stderr, fmt.c_str(), args...);
	}

	template<typename ...Args>
	static void error(const std::string& fmt, Args... args) {
		print(MessageLevelBit::eERROR, fmt, args...);
#ifdef _DEBUG
		throw fmt;
#endif
	}

	template<typename ...Args>
	static void info(const std::string& fmt, Args... args) {
		print(MessageLevelBit::eINFO, fmt, args...);
	}

	template<typename ...Args>
	static void debug(const std::string& fmt, Args... args) {
		print(MessageLevelBit::eDEBUG, fmt, args...);
	}
}