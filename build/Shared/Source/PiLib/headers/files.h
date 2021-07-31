#pragma once

#define INCLUDE_ALL
#include "include.h"
#undef INCLUDE_ALL

#include "utility.h"
#include "info.h"

namespace pilib {
	struct WinSync {
		std::string name;
		std::string source;
		std::string destination;
		std::string options;
	};

	typedef std::vector<std::string> Line;
	typedef std::vector<Line> Csv;
	//util::csv::Csv = 'std::vector< std::vector<std::string> >'

	Csv csvRead(const char* filepath);
	void csvRead(const char* filepath, Csv& container);

	void winSync(const char* filepath, std::ostream& output);
}