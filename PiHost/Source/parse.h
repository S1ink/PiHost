#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include "basic.h"

#include "unix/syscom.h"

struct WinSync {
	std::string name;
	std::string source;
	std::string destination;
	std::string options;
};

void winSync(const char* filepath, std::ostream& output);