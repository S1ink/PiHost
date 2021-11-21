#pragma once

#include "../STD.h"
#include "../pivar.h"
#include "../Runtime.h"
#include "../Utility.h"
#include "../System.h"

#include "output.h"

namespace pilib {
	typedef std::vector<std::string> Line;
	typedef std::vector<Line> Csv;
	//Csv = 'std::vector< std::vector<std::string> >'
	struct WinSync {
		std::string name;
		std::string source;
		std::string destination;
		std::string options;
	};

	Csv csvRead(const char* filepath);
	void csvRead(const char* filepath, Csv& container);

	void winSync(const char* filepath, std::ostream& output);



	typedef void(*TaskFunc)(const char*, std::ostream&);
	struct TaskFile {
		std::string name;
		std::string command;
		std::string output;
		std::string mode;
		DayTime tme;
	};

	/*void streamWrapper(const char* message, std::ostream const& output, TaskFunc func);
	void streamWrapper(const char* message, pilib::lstream output, TaskFunc func);
	void parseTasks(const char* filepath, std::ostream& output, std::atomic_bool& control, time_t th_uintv, std::vector<std::thread>& threads, std::unordered_map<std::string, TaskFunc>& funcmap);*/
}