#pragma once

#include "../STD.h"
#include "../pivar.h"

namespace pilib {
	void exec(const char* command, std::ostream& output);

	int aptUpdate();
	int aptUpdate(std::ostream& out);
	int aptUpgrade();
	int aptUpgrade(std::ostream& out);

	void rsync(std::ostream& output, const char* source, const char* destination, const char* options = "-va");
	void rclone(std::ostream& output, const char* source, const char* destination, const char* mode = "sync");
	void rsync(const char* source, const char* destination, const char* options = "-a");
	void rclone(const char* source, const char* destination, const char* mode = "sync");
}