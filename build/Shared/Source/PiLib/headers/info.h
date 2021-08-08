#pragma once

#define INCLUDE_ALL
#include "include.h"
#undef INCLUDE_ALL

namespace pilib {
	class SYS {
		// all stats here
	};

	class CPU {
	private:
		class CoreData {
		private:
			enum class States {
				USER, NICE, SYSTEM,
				IDLE, IOWAIT, IRQ,
				SOFTIRQ, STEAL,
				GUEST, GUEST_NICE,
				TOTAL
			};
			static const std::array<States, 2> s_idle;
			static const std::array<States, 8> s_active;
		public:
			std::string title;
			uint data[(uint)States::TOTAL];

			void update();
			//create option to select line to update from

			CoreData(){}
			CoreData(void*);
			CoreData(const CoreData& other);

			uint getIdle();	//return value is in usr_hz -> the amount to time chunks in said state
			uint getActive();
			uint getTotal();
			uint getState(States state);

			static void readAll(std::vector<CoreData>& lines);
		};

		uint c_cores;
		CoreData *cbuff1, *cbuff2;
	public:
		//typedef std::pair<std::string, float> Util;
		typedef std::map<std::string, float> UtilMap;

		CPU();
		~CPU();

		uint count();
		//static uint count();
		
		static float average(CoreData& first, CoreData& second);
		static void averageVec(std::vector<CoreData>& first, std::vector<CoreData>& second, UtilMap& out);
		static UtilMap averageVec(std::vector<CoreData>& first, std::vector<CoreData>& second);

		static float temp();

		template<typename rep, typename period>
		static float percent(CHRONO::duration<rep, period> interval) {
			CoreData second, first(nullptr);
			std::this_thread::sleep_for(interval);
			second.update();
			return average(first, second);
		}
		static float percent(int seconds = 1);

		template<typename rep, typename period>
		static UtilMap percentMap(CHRONO::duration<rep, period> interval) {
			std::vector<CoreData> first, second;
			CoreData::readAll(first);
			std::this_thread::sleep_for(interval);
			CoreData::readAll(second);
			return averageVec(first, second);
		}
		static UtilMap percentMap(int seconds = 1);

		static float searchUtil(const std::string& search, UtilMap& map);
		//static float searchUtil(const std::string& item);
	};

	class NET {
	private:

	public:

	};

	//old
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

/* CPU States
user – time spent in user mode.
nice – time spent in user mode with low priority.
system – time spent in system mode.
idle – time spent in the idle task.
iowait –  time waiting for I/O to complete.
irq – time servicing hardware interrupts.
softirq – time servicing software interrupts.
steal – time spent in other operating systems when running in a virtualized environment.
guest – time spent running a virtual CPU for guest operating systems.
guest_nice – time spent running a low priority virtual CPU for guest operating systems.
* * * *
idle + iowait = time doing nothing (usr_hz)
all others = time spend utilized
*/