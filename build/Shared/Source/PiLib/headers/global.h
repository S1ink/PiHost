#pragma once

#define INCLUDE_ALL
#include "include.h"
#undef INCLUDE_ALL
namespace pilib {
	class Globals {	//instantiates and contains all global singletons
	public:


		Globals(){}
		~Globals(){}
	};

	class Pdir {
	private:
		Pdir() {}
		Pdir(const Pdir&) = delete;

		//char that determines directory separator
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
		std::string* intDir();

		static std::string directory(const std::string& fullpath);
		static std::string s_directory(const std::string& fullpath);
	};
	extern Pdir& proot;

	class Debug {
	private:
		Debug() {}
		Debug(const Debug&) = delete;

		std::ostream* stream;
	public:
		static Debug& get() {
			Debug global;
			return global;
		}
	};
}