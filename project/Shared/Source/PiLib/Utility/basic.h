#pragma once

#include "../STD.h"
#include "../pivar.h"

namespace pilib {
	uint16_t toNum(char num);
	char clearEnd(std::string& str);
	void replace(std::string& str, const std::string find, const char* replace);
	std::streampos lineNum(uint line, std::istream& stream);
	std::streampos lineNum(uint line, std::istream& stream, std::string& buf);

	bool exists(const char* path);
	bool exists(const std::string& path);

	std::string withTime(const char* message);

	const char* dateStamp();
	const char* dateStamp(time_t* tme);

	template<typename input>
	void debug(input identifier) {
		std::cout << dateStamp() << " : DEBUG: " << identifier << newline;
	}

	void exitError(const char* message);
}
