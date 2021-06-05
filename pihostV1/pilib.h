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

namespace sysinfo {
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
	void appLog(std::string file, std::string text);
}

namespace gpio {
	int getStatus();
	int getSwitch();
	void activateSwitch(int id);
	void init();
	void gpio1();
}