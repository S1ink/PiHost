#include "program.h"

namespace pilib {
	const char* Pdir::setDir(const std::string& pname) {
		this->dir = pname.substr(0, pname.find_last_of(s_dir));
		return this->dir.c_str();
	}
	const char* Pdir::getDir() {
		return this->dir.c_str();
	}
	std::string Pdir::getDirSlash() {
		return (this->dir + s_dir);
	}
	std::string Pdir::getItem(const std::string& itempath) {
		return this->dir + itempath;
	}
	std::string Pdir::getRawItem(const std::string& item) {
		return (this->dir + s_dir + item);
	}
	std::string* Pdir::intDir() {
		return &(this->dir);
	}

	std::string Pdir::directory(const std::string& fullpath) {
		return fullpath.substr(0, fullpath.find_last_of(s_dir));
	}
	std::string Pdir::s_directory(const std::string& fullpath) {
		return fullpath.substr(0, fullpath.find_last_of(s_dir) + 1);
	}

	Pdir& progdir = Pdir::get();
}