#pragma once

#include "pigpio.h"
#include "pivar.h"

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
#include <vector>
#include <sstream>

namespace info {
	float cputemp();
	float gputemp();

	namespace cpu {
		constexpr int parsable_states = 10;

		struct LineParse {
			std::string title;
			size_t time[parsable_states];
		};

		struct ActivityData {
			std::string title;
			float activity;
		};

		size_t getIdle(const LineParse& data);
		size_t getActive(const LineParse& data);
		void readMain(LineParse& container);
		void readVector(std::vector<LineParse>& container);
		void convertData(LineParse& snapshot1, LineParse& snapshot2, ActivityData& converted);
		void convertVectorData(std::vector<LineParse>& snapshot1, std::vector<LineParse>& snapshot2, std::vector<ActivityData>& converted);
		void takeSample(std::vector<LineParse>& vector1, std::vector<LineParse>& vector2, std::vector<ActivityData>& result, unsigned int sample_interval = 1);
		float getPercent(unsigned int wait = 1);
	}

	class Cpu {
		//const char* path = locations::stats::cpu;
	public:
		int sample_rate;

		std::vector<info::cpu::LineParse> sample1;
		std::vector<info::cpu::LineParse> sample2;
		std::vector<info::cpu::ActivityData> result;

		Cpu(unsigned int sample_time = 1);

		std::vector<info::cpu::ActivityData>& getSample();
	};

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
	char* cutNewline(char* text);
	void singleLog(std::string file, std::string text);
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
	constexpr int p_fan = 18;
	constexpr int p_switch = 3;
	constexpr int p_power = 23;
	constexpr int p_reset = 24;
	constexpr int p_status = 25;

	int getStatus();
	int getSwitch();
	void activateSwitch(int id);
	void init();
	void setNoctua(int percent);
}