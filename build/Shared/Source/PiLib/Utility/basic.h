#pragma once

#include "../STD.h"
#include "../pivar.h"

namespace pilib {
	char clearEnd(std::string& str);
	void replace(std::string& str, const std::string find, const char* replace);
	bool exists(const char* path);

	std::string withTime(const char* message);

	const char* dateStamp();
	const char* dateStamp(time_t* tme);

	template<typename input>
	void debug(input identifier) {
		std::cout << dateStamp() << " : DEBUG: " << identifier << newline;
	}

	void exitError(const char* message);
}
