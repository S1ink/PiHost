#pragma once

#include "pigpio.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <ctime>
#include <chrono>
#include <stdio.h>
#include <cstdio>
#include <memory>
#include <array>
#include <string.h>
#include <stdexcept>

constexpr int p_fan = 18;
constexpr int p_switch = 3;
constexpr int p_power = 23;
constexpr int p_reset = 24;
constexpr int p_status = 25;

constexpr char newline = '\n';
constexpr char csvsep = ',';

namespace info {
	float cputemp();
	float gputemp();
	time_t now();
	std::chrono::time_point<std::chrono::system_clock> _now();
	tm* struct_now();
	tm* _struct_now();
	char* dateStamp();
	double perf_timer(time_t start);
	double elapsed_time(std::chrono::time_point<std::chrono::system_clock> start);
}

namespace util {
	std::string exec(std::string command);
	char* returnCut(char* text);
	void gLog(std::string file, std::string text);
	class logger {
	private:
		std::fstream scribe;
		std::string path;
		std::ios_base::openmode modes;
	public:
		logger(const char* path, std::ios_base::openmode modes) : path(path), modes(modes) {};
		void log(std::string text);
		void stampedLog(std::string text);
	};
}

namespace gpio {
	int getStatus();
	int getSwitch();
	void activateSwitch(int id);
	void init();
	void setNoctua(int percent);
}