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
#include <atomic>
#include <map>
#include <typeinfo>
#include <sys/socket.h>
#include <netinet/in.h>
#include <algorithm>
#include <regex>

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
		//change sleep metric to CHRONO >>
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
	void exec(const char* command, std::ostream& output);
	int aptUpdate();
	int aptUpdate(std::ostream& out);
	int aptUpgrade();
	int aptUpgrade(std::ostream& out);
	std::string rsync(const char* source, const char* destination, const char* options = "-va");
	std::string rclone(const char* source, const char* destination, const char* mode = "sync");
	void s_rsync(const char* source, const char* destination, const char* options = "-a");
	void s_rclone(const char* source, const char* destination, const char* mode = "sync");

	unsigned char clearEnd(std::string& str);
	void replace(std::string& str, const std::string find, const char* replace);
	void quickLog(std::string file, std::string text);

	template<typename input>
	void debug(input identifier) {
		std::cout << "DEBUG: " << identifier << newline;
	}

	void error(const char* message);

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
		void pTotal(std::ostream& output);
	};

	typedef unsigned int time_d;
	time_d createTOD(uint16_t hr, uint16_t min, uint16_t sec);
	struct DayTime {
		uint16_t hr;
		uint16_t min;
		uint16_t sec;

		DayTime(const uint16_t hr = 0, const uint16_t min = 0, const uint16_t sec = 0) : hr(hr), min(min), sec(sec) {}
		time_d toTOD();
	};

	time_t d_untilNext(const DayTime tme);
	time_t d_untilNext(DayTime& tme);
	CHRONO::time_point<CHRONO::system_clock> d_nextTime(const DayTime tme);
	CHRONO::time_point<CHRONO::system_clock> d_nextTime(DayTime& tme);
}

namespace files {
	namespace csv {
		struct WinSync {
			std::string name;
			std::string source;
			std::string destination;
			std::string options;
		};

		typedef std::vector<std::string> Line;
		typedef std::vector<Line> Csv;
		//util::csv::Csv = 'std::vector< std::vector<std::string> >'

		Csv csvRead(const char* filepath);
		void csvRead(const char* filepath, Csv& container);

		bool winCheck(const char* filepath);
		void winSync(const char* filepath, std::ostream& output);
	}

	struct TaskFile {
		std::string name;
		std::string command;
		std::string output;
		std::string mode;
		timing::DayTime tme;
	};
}

namespace threading {
	template<typename returntype, typename... args>
	void routineThread(std::atomic_bool const& control, const timing::DayTime tme, const time_t uintv, returntype(*func)(args...), args... arg) {
		std::atomic_bool& con = const_cast<std::atomic_bool&>(control);
		while (con) {
			while (uintv < d_untilNext(tme)) {
				if (!con) {
					break;
				}
				std::this_thread::sleep_for(CHRONO::seconds(uintv));
			}
			if (con) {
				std::this_thread::sleep_until(d_nextTime(tme));
				func(arg...);
				std::this_thread::sleep_for(CHRONO::seconds(1));
			}
		}
	}

	template<typename d_rep, typename d_period, typename returntype, typename... args>
	void loopingThread(std::atomic_bool const& control, CHRONO::duration<d_rep, d_period> interval, CHRONO::duration<d_rep, d_period> uinterval, returntype(*func)(args...), args... arg) {
		std::atomic_bool& con = const_cast<std::atomic_bool&>(control);
		int ratio = CHRONO::duration_cast<CHRONO::seconds>(interval) / CHRONO::duration_cast<CHRONO::seconds>(uinterval);
		if (ratio > 1) {
			CHRONO::duration<d_rep, d_period> rest = interval % uinterval;
			while (con) {
				for (int i = 0; i < ratio; i++) {
					if (!con) {
						break;
					}
					std::this_thread::sleep_for(uinterval);
				}
				if (con) {
					std::this_thread::sleep_for(rest);
					func(arg...);
				}
			}
		}
		else {
			while (con) {
				std::this_thread::sleep_for(interval);
				func(arg...);
			}
		}
	}

	typedef void(*templatefunc)(const char*, std::ostream&);

	void streamWrapper(const char* message, std::ostream const& output, templatefunc func);
	void parseTasks(const char* filepath, std::ostream& output, std::atomic_bool& control, time_t th_uintv, std::vector<std::thread>& threads, std::map<std::string, templatefunc>& funcmap);
}

namespace networking {
	//https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa

	void sendHtml(std::string& request, std::ostream& output); //ignores request, sends payload no matter what
	void sendBinary(std::string& request, std::ostream& output);
	void errorPage(const char* file, const char* code, std::ostream& output); //return an html error page with inserted error code
	void simpleRequest(std::string& request, std::ostream& output); //supports GET and HEAD requests only

	class Server {
	private:
		sockaddr_in address;
		int sock, nsock, max_users, addrlen;
	public:
		typedef void(*responsehandler)(std::string&, std::ostream&);
		std::string root;

		Server(int connections = 5, int domain = AF_INET, int service = SOCK_STREAM, int protocol = 0, int port = 80, ulong interface = INADDR_ANY);
		void bindServer();
		void startListen();
		void prep();
		void launch(std::atomic_bool& condition, std::ostream& output, Server::responsehandler handler);

		std::string fullPath(const char* file);
		void basicHeader(std::ostream& output, const char* type, std::ostringstream& resource);

		void internalHandler(std::string& request, std::ostream& output);
	};
}

namespace gpio {
	int getStatus();
	int getSwitch();
	void activateSwitch(int id);
	void init(float fanspeed = 40.f);
	void setNoctua(float percent);
}