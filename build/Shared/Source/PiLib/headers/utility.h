#pragma once

#define INCLUDE_ALL
#include "include.h"
#undef INCLUDE_ALL

#include "info.h"

namespace pilib {
	//lstream
	/*class dstream : public std::basic_ostream<char> {
	private:

	public:
		dstream(const std::ostream& type);
	};*/

	//iofstream
	class lstream : public std::basic_fstream<char> {
#define OMODE std::_Ios_Openmode
	private:
		std::string file;
		OMODE modes;
	public:
		lstream() : std::basic_fstream<char>() {}
		lstream(const char* file, OMODE modes = (std::ios::in | std::ios::out)) : 
			std::basic_fstream<char>(), file(file), modes(modes) {}
		lstream(const char* file, uint8_t init, OMODE modes = (std::ios::in | std::ios::out)) : 
			std::basic_fstream<char>(file, modes), file(file), modes(modes) {}

		void setData(const char* file, OMODE modes = (std::ios::in | std::ios::out));
		void setData(OMODE modes);

		const char* getFile();
		OMODE getModes();
		std::string* rawFile();
		OMODE* rawModes();

		void openFile(const char* file, OMODE modes = (std::ios::in | std::ios::out));
		void openInput();
		void openOutput();

		lstream(const lstream& other);
		//lstream(lstream&& other);
		//~lstream();
#undef OMODE
	};

	//supports bool, char, int, uint, float, str(std)
	class Data {
	private:
		typedef void(Data::* extractor)(const char*);

		void* data;	//smart?
		extractor ext;

		void extractBool(const char* str);
		void extractChar(const char* str);
		void extractInt(const char* str);
		void extractUint(const char* str);
		void extractFloat(const char* str);
		void extractLong(const char* str);
		void extractStr(const char* str);
	public:
		//Data() {}
		Data(bool* item) : data(item), ext(&Data::extractBool) {}
		Data(char* item) : data(item), ext(&Data::extractChar) {}
		Data(int* item) : data(item), ext(&Data::extractInt) {}
		Data(uint* item) : data(item), ext(&Data::extractUint) {}
		Data(float* item) : data(item), ext(&Data::extractFloat) {}
		Data(long* item) : data(item), ext(&Data::extractLong) {}
		Data(std::string* item) : data(item), ext(&Data::extractStr) {}

		void extract(const char* str);
	};

	class ArgsHandler {
	private:
		ArgsHandler() {}
		ArgsHandler(const ArgsHandler&) = delete;
		//~move

		std::unordered_map<std::string, Data> vars;
	public:
		static ArgsHandler& get();

		std::unordered_map<std::string, Data>* getVars();
		void insertVars(std::initializer_list< std::pair<const std::string, Data> > list);
		uint parse(int argc, char* argv[]);
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
	bool exists(const char* path);

	std::string relpath(const char* filepath);
	std::string relpath(const std::string& filepath);
	
	template<typename input>
	void debug(input identifier) {
		std::cout << "DEBUG: " << identifier << newline;
	}

	void exitError(const char* message);
}