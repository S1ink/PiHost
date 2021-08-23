#pragma once

#include "../STD.h"
#include "../Utility.h"

namespace pilib {
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
}