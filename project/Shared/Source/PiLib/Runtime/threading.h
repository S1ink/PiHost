#pragma once

#include "../STD.h"
#include "../pivar.h"

#include "timing.h"

namespace pilib {
	template<typename returntype, typename... args>
	void routineThread(const std::atomic_bool& control, const DayTime tme, const time_t uintv, returntype(*func)(args...), args... arg) {
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
	template<typename returntype, typename... args>
	void p_routineThread(std::atomic_bool** control, const DayTime tme, const time_t uintv, returntype(*func)(args...), args... arg) {
		while (**control) {
			while (uintv < d_untilNext(tme)) {
				if (!**control) {
					break;
				}
				std::this_thread::sleep_for(CHRONO::seconds(uintv));
			}
			if (**control) {
				std::this_thread::sleep_until(d_nextTime(tme));
				func(arg...);
				std::this_thread::sleep_for(CHRONO::seconds(1));
			}
		}
	}

	// >> another version of the above but with CHRONO time implemented <<

	//this function starts the interval to wait after the desired func finishes, so routines will be altered by however long the function takes
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
}