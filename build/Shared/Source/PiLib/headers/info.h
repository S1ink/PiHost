#pragma once

#define INCLUDE_ALL
#include "include.h"
#undef INCLUDE_ALL

namespace pilib {
	namespace sys {
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

			template<typename rep, typename period>
			void takeSample(std::vector<LineParse>& vector1, std::vector<LineParse>& vector2, std::vector<ActivityData>& result, CHRONO::duration<rep, period> interval) {
				vector1.clear();
				vector2.clear();
				result.clear();
				readVector(vector1);
				std::this_thread::sleep_for(interval);
				readVector(vector2);
				convertVectorData(vector1, vector2, result);
			}
		}

		float cpuTemp();
		float gpuTemp();

		float cpuPercent(CHRONO::duration<CHRONO::seconds::rep, CHRONO::seconds::period> interval);
	}

	char* dateStamp();
	char* dateStamp(time_t* tme);
}