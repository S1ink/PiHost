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

	char* dateStamp();
	char* dateStamp(time_t* tme);
}

namespace util {
	std::string exec(const char* command);
	int aptUpdates();
	int aptUpdates(std::ostream& out);
	int aptUpgrade();
	int aptUpgrade(std::ostream& out);
	std::string rsync(const char* source, const char* destination, const char* options = "-va");
	std::string rclone(const char* source, const char* destination, const char* mode = "sync");
	void s_rsync(const char* source, const char* destination, const char* options = "-a");
	void s_rclone(const char* source, const char* destination, const char* mode = "sync");
	char* cutNewline(char* text);
	void quickLog(std::string file, std::string text);
	void debug(const char* identifier);

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

namespace timing {
	time_t now();
	tm* struct_now();
	double perfTimer(CHRONO::time_point<CHRONO::high_resolution_clock>& start);

	class StopWatch {
		CHRONO::time_point<CHRONO::high_resolution_clock> start;
	public:
		StopWatch(bool now = true);
		void setStart();
		double getDuration();
		void pTotal();
	};

	typedef unsigned int time_d;
	time_d createTOD(uint8_t hr, uint8_t min, uint8_t sec);
	struct DayTime {
		uint8_t hr;
		uint8_t min;
		uint8_t sec;

		DayTime(const uint8_t hr = 0, const uint8_t min = 0, const uint8_t sec = 0) : hr(hr), min(min), sec(sec) {}
		time_d toTOD();
	};

	time_t d_untilNext(const DayTime tme);
	CHRONO::time_point<CHRONO::system_clock> d_nextTime(const DayTime tme);

	template<typename returntype, typename... args>
	void routineThread(volatile bool const& control, const DayTime tme, returntype(*func)(args...), args... arg) {
		bool& con = const_cast<bool&>(control);
		while (con) {
			std::this_thread::sleep_until(d_nextTime(tme));
			func(arg...);
			std::this_thread::sleep_for(CHRONO::seconds(1));
		}
	}

	template<typename d_rep, typename d_period, typename returntype, typename... args>
	void loopingThread(volatile bool const& control, CHRONO::duration<d_rep, d_period> interval, returntype(*func)(args...), args... arg) {
		bool& con = const_cast<bool&>(control);
		while (con) {
			std::this_thread::sleep_for(interval);
			func(arg...);
		}
	}
}

namespace files {
	namespace csv {
		struct WinSync {
			std::string name;
			std::string source;
			std::string destination;
			std::string options;
		};

		struct TaskFile {
			std::string name;
			std::string command;
			std::string output;
			timing::DayTime tme;
		};

		typedef std::vector<std::string> Line;
		typedef std::vector<Line> Csv;
		//util::csv::Csv = 'std::vector< std::vector<std::string> >'

		Csv csvRead(const char* filepath);
		void csvRead(const char* filepath, Csv& container);

		bool winCheck(const char* filepath);
		void winSync(const char* filepath, std::ostream& output);

		void parseTasks(const char* filepath, std::ostream& output);
	}
}

namespace gpio {
	int getStatus();
	int getSwitch();
	void activateSwitch(int id);
	void init(float fanspeed = 40.f);
	void setNoctua(float percent);
}