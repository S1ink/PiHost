#pragma once

#include "include.h"

namespace pilib {
	time_t now();
	tm* struct_now();
	double perfTimer(CHRONO::time_point<CHRONO::high_resolution_clock>& start);

	class StopWatch {
		CHRONO::time_point<CHRONO::high_resolution_clock> ref;
	public:
		StopWatch(bool now = true);
		void setStart();
		double getDuration();
		void pTotal(std::ostream& output = std::cout);
		~StopWatch();
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