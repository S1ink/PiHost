#pragma once

#include "../STD.h"
#include "../pivar.h"

namespace pilib {
	class Pdir {
	private:
		Pdir() {}
		Pdir(const Pdir&) = delete;

		std::string dir;
	public:
		static Pdir& get() {
			static Pdir global;
			return global;
		}

		const char* setDir(const std::string& pname);
		const char* getDir();
		std::string getDirSlash();
		std::string getItem(const std::string& itempath);
		std::string getRawItem(const std::string& item);
		std::string* intDir();

		static std::string directory(const std::string& fullpath);
		static std::string s_directory(const std::string& fullpath);
	};
	extern Pdir& progdir;
}