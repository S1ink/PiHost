#pragma once

#include <iostream>
#include <vector>
#include <atomic>
#include <unordered_map>
#include <thread>
#include <string>
#include <sstream>

#include "output.h"
#include "timing.h"
#include "basic.h"
#include "threading.h"
#include "resources.h"

#include "PiLib/System/syscom.h"

//#define TESTING

typedef void(*Task_f)(const char*, const olstream&);

class TaskManager {
private:
	struct Task_t {
		std::string name;
		std::string output;
		std::string w_mode;
		DayTime tme;
		std::string args;
	};

	std::string tfile;
	std::vector<std::thread> threads;
	std::atomic_bool* external;
	olstream output;
	std::unordered_map<std::string, Task_f> funcmap;
	const time_t clock;	//seconds

	std::thread launched;
protected:
	static void launchWorker(TaskManager* that);
public:
	TaskManager(
		std::string&& path, 
		std::atomic_bool& rbool, 
		const std::unordered_map<std::string, Task_f>& funcs, 
		const olstream& out = &std::cout, 
		time_t clock_intv = 10
	);
	TaskManager(
		std::string&& path, 
		std::atomic_bool& rbool, 
		const std::unordered_map<std::string, Task_f>& funcs, 
		olstream&& out = &std::cout, 
		time_t clock_intv = 10
	);
	TaskManager(
		const std::string& path,
		std::atomic_bool& rbool,
		const std::unordered_map<std::string, Task_f>& funcs,
		const olstream& out = &std::cout,
		time_t clock_intv = 10
	);
	//TaskManager(const TaskManager& other);
	//TaskManager(TaskManager&& ohter);
	~TaskManager();

	//TaskManager& operator=(const TaskManager& other);
	//TaskManager& operator=(TaskManager&& other);

	void insertFunc(std::initializer_list< std::pair<const std::string, Task_f> > map);

	void launch();	//take in an optional "TaskManager" instance for updated params?
	void end();

	size_t getThreads();

	static void runCommand(const char* message, const olstream& logs);
	static void errorName(const char* message, const olstream& logs);
};