#pragma once

#define INCLUDE_ALL
#include "include.h"
#undef INCLUDE_ALL

namespace pilib {
#define OMODE std::_Ios_Openmode
	class olstream {
	private:
		bool fmode;	//true for file, false for stream
		std::ofstream file;
		std::ostream* stream;
		const char* fname;
		OMODE omodes;

		inline bool checkStream();
		std::ostream& safeStream();
	public:
		olstream();
		olstream(const char* file, const OMODE modes = std::ios::out);
		olstream(std::ostream* stream);
		olstream(const olstream& other);

		void setStream(const char* file, const OMODE modes = std::ios::out);
		void setStream(std::ostream* stream = &std::cout);

		/*bool open();
		template<typename t>
		void fwrite(t item) {

		}*/

		template<typename t>
		olstream& operator<<(t item) {
			if (this->fmode) {
				this->file.open(this->fname, ((this->omodes | std::ios::out) & ~std::ios::in));
				this->file << item;
				this->file.close();
			}
			else {
				this->safeStream() << item;
			}
			return *this;
		}

		//for more efficient file operations
		template<typename t>
		olstream& operator<<=(t item) {
			if (this->fmode) {
				this->file.open(this->fname, ((this->omodes | std::ios::out) & ~std::ios::in));
				this->file << item;
			}
			else {
				checkStream();
				*(this->stream) << item;
			}
			return *this;
		}
		template<typename t>
		olstream& operator<=(t item) {
			if ((this->fmode) && this->file.is_open()) {
				this->file << item;
			}
			else {
				*(this->stream) << item;
			}
			return *this;
		}
		template<typename t>
		olstream& operator<(t item) {
			if (this->fmode && this->file.is_open()) {
				this->file << item;
				this->file.close();
			}
			else {
				*(this->stream) << item;
			}
			return *this;
		}
	};

	//template to pass to superclasses?
	class lstream : public std::basic_fstream<char> {
	private:
		const char* file;
		OMODE modes;
	public:
		lstream() : basic_fstream<char>() {}	//default

		lstream(const char* file, const OMODE modes = (std::ios::in | std::ios::out)) :	//file stream mode
			basic_fstream<char>(), file(file), modes(modes) {}

		lstream(const char* file, uint8_t init, const OMODE modes = (std::ios::in | std::ios::out)) :	//file stream mode, open by default
			basic_fstream<char>(file, modes), file(file), modes(modes) {}

		//lstream(std::ostream& stream, const OMODE modes = std::ios::out);	//from basic_ostream
		//lstream(

		lstream(const lstream& other);	//copy constructor
		//lstream(lstream&& other);
		//~lstream();

		template<typename ct>
		std::streambuf* rdbuf(std::basic_streambuf<char, ct>* sb) {
			std::streambuf* ret = this->std::basic_fstream<char>::rdbuf();
			this->set_rdbuf(sb);
			return ret;
		}

		void setFile(const char* file, OMODE modes = (std::ios::in | std::ios::out));
		void setMode(OMODE modes);
		const char* getFile();
		OMODE getModes();

		void openFile(const char* file, OMODE modes = (std::ios::in | std::ios::out));
		void openInput();
		void openOutput();

		/*template<typename t>
		lstream& operator<<(t& data) {

		}

		template<typename t>
		lstream& operator>>(t& data) {

		}*/
	};
#undef OMODE

	template<typename t>
	lstream& operator<<(lstream& stream, t& item) {
		//std::cout << "overloaded!\n";
		stream.openOutput();
		static_cast<std::ostream&>(stream) << item;
		stream.close();
		return stream;
	}

	class SigHandle {
	private:
		typedef void(*sigfunc)(int);

		static const std::array<const char*, 31> sigmap;
		std::vector<uint> ign_sigs, exit_sigs;
		sigfunc ign, ex;

		SigHandle() :
			ign_sigs({ 1, 5, 10, 12, 13, 14, 17, 18, 20, 21, 22, 23, 26, 27, 28, 29, 31 }),
			exit_sigs({ 2, 3, 4, 6, 7, 8, 11, 15, 16, 24, 25, 30 }) {}
		SigHandle(const SigHandle&) = delete;
	public:
		static SigHandle& get() {
			static SigHandle global;
			return global;
		}

		static void sigIgnore(int signum);
		static void sigTerminate(int signum);
		static void ignoreBase(int signum, std::ostream& out = std::cout);
		static void terminateBase(int signum, std::ostream& out = std::cout);

		//void ignore(std::initializer_list<uint> list);
		//void exit(std::initializer_list<uint> list);
		void reset(std::initializer_list<uint> ingore, std::initializer_list<uint> exit);
		void setup(sigfunc ignore, sigfunc exit);
		void setup(sigfunc ignore, sigfunc exit, std::initializer_list<uint> ignorelist, std::initializer_list<uint> exitlist);
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
	
	std::string withTime(const char* message);

	const char* dateStamp();

	template<typename input>
	void debug(input identifier) {
		std::cout << dateStamp() << " : DEBUG: " << identifier << newline;
	}

	void exitError(const char* message);
}

/*
	class lstream : public std::basic_fstream<char>, public std::basic_iostream<char> {
#define OMODE std::_Ios_Openmode
	private:
		std::string file;
		OMODE modes;
	public:
		lstream() : std::basic_fstream<char>(), std::basic_iostream<char>() {}
		lstream(const char* file, OMODE modes = (std::ios::in | std::ios::out)) : 
			std::basic_fstream<char>(), file(file), modes(modes) {}
		lstream(const char* file, uint8_t init, OMODE modes = (std::ios::in | std::ios::out)) : 
			std::basic_fstream<char>(file, modes), file(file), modes(modes) {}
		lstream(const lstream& other);
		//lstream(lstream&& other);
		//~lstream();

		void setData(const char* file, OMODE modes = (std::ios::in | std::ios::out));
		void setData(OMODE modes);

		const char* getFile();
		OMODE getModes();
		std::string* rawFile();
		OMODE* rawModes();

		void openFile(const char* file, OMODE modes = (std::ios::in | std::ios::out));
		void openInput();
		void openOutput();
#undef OMODE
	};
*/