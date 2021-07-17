#pragma once

#include "include.h"

namespace pilib {
	class lstream : public std::basic_fstream<char> {
#define OMODE std::_Ios_Openmode
	private:
		std::string file;
		OMODE modes;
	public:
		lstream() : std::basic_fstream<char>() {}
		lstream(const char* file, OMODE modes = (std::ios::in | std::ios::out)) : std::basic_fstream<char>(), file(file), modes(modes) {}
		lstream(const char* file, uint8_t init, OMODE modes = (std::ios::in | std::ios::out)) : std::basic_fstream<char>(file, modes), file(file), modes(modes) {}

		void setData(const char* file, OMODE modes = (std::ios::in | std::ios::out));
		void setData(OMODE modes);

		const char* getFile();
		OMODE getModes();
		std::string* rawFile();
		OMODE* rawModes();

		void openFile(const char* file, OMODE modes = (std::ios::in | std::ios::out));
		void openInput();
		void openOutput();

		/*lstream(lstream const& other);
		lstream(lstream&& other);*/
		//~lstream();
#undef OMODE
	};

	void exec(const char* command, std::ostream& output);
	int aptUpdate();
	int aptUpdate(std::ostream& out);
	int aptUpgrade();
	int aptUpgrade(std::ostream& out);
	void rsync(std::ostream& output, const char* source, const char* destination, const char* options = "-va");
	void rclone(std::ostream& output, const char* source, const char* destination, const char* mode = "sync");
	void rsync(const char* source, const char* destination, const char* options = "-a");
	void rclone(const char* source, const char* destination, const char* mode = "sync");

	char clearEnd(std::string& str);
	void replace(std::string& str, const std::string find, const char* replace);

	template<typename input>
	void debug(input identifier) {
		std::cout << "DEBUG: " << identifier << newline;
	}

	void error(const char* message);
}