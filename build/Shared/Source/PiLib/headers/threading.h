#pragma once

#define INCLUDE_ALL
#include "include.h"
#undef INCLUDE_ALL

#include "timing.h"
#include "utility.h"

namespace pilib {
	template<typename returntype, typename... args>
	void routineThread(std::atomic_bool const& control, const DayTime tme, const time_t uintv, returntype(*func)(args...), args... arg) {
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

	struct TaskFile {
		std::string name;
		std::string command;
		std::string output;
		std::string mode;
	    DayTime tme;
	};

	void streamWrapper(const char* message, std::ostream const& output, templatefunc func);
	void streamWrapper(const char* message, pilib::lstream output, templatefunc func);
	void parseTasks(const char* filepath, std::ostream& output, std::atomic_bool& control, time_t th_uintv, std::vector<std::thread>& threads, std::unordered_map<std::string, templatefunc>& funcmap);
}