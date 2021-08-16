#include "headers/global.h"

namespace pilib {
	const char* Pdir::setDir(const std::string& pname) {
		this->dir = pname.substr(0, pname.find_last_of('/'));
		return this->dir.c_str();
	}
	const char* Pdir::getDir() {
		return this->dir.c_str();
	}
	std::string Pdir::getDirSlash() {
		std::string ret = (this->dir + '/');
		return ret;
	}
	std::string Pdir::getItem(const std::string& itempath) {
		return this->dir + itempath;
	}
	std::string* Pdir::intDir() {
		return &(this->dir);
	}

	std::string Pdir::directory(const std::string& fullpath) {
		return fullpath.substr(0, fullpath.find_last_of('/'));
	}
	std::string Pdir::s_directory(const std::string& fullpath) {
		return fullpath.substr(0, fullpath.find_last_of('/')+1);
	}

	Pdir& proot = Pdir::get();
}