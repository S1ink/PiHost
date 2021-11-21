#pragma once

#include "../STD.h"
#include "../pivar.h"

//namespace stats {
//	CE_STR cpu = "/proc/stat";
//	CE_STR network = "/proc/net/dev";
//	CE_STR load = "/proc/loadavg";
//	CE_STR disk = "/proc/diskstats";
//	CE_STR mem = "/proc/meminfo";
//}

namespace pilib {
	//class SYS {
	//	// all stats here
	//};

	class CPU {
	public:
#define E_CHECKS 1
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

		protected:
			std::string title;
			std::streampos hint;
			std::array<uint, (size_t)States::TOTAL> data;

			void parse(std::ifstream& reader);
			void parse(const std::streampos& begin);
			void parse(std::ifstream& reader, const std::streampos& begin);

			void fromMain();
			void fromPos(const std::streampos& begin);
			void fromTag(const std::string& tag);
			void fromNum(uint corenum);

		public:
			void update();	//updates from main CPU stats
			//void update(std::istream& file);
			uint updateFrom(uint core);	//updates from specified core (starting at 1, main is 0), if no core is found, updates from main
			//uint updateFrom(uint core, std::istream& file);
			void parseFrom(const std::string& line);

			CoreData() {}
			CoreData(uint core);
			CoreData(const std::string& line);
			CoreData(const CoreData& other);
			CoreData(CoreData&& other);

			CoreData& operator=(const CoreData& other);
			CoreData& operator=(CoreData&& other);

			const std::string& getTitle();
			uint getIdle();	//return value is in usr_hz -> the amount of time chunks in said state
			uint getActive();
			uint getTotal();
			uint getState(States state);
			uint getStates(States states[], size_t size);

			static void readAll(std::vector<CoreData>& lines);
		};

		template <size_t cc>
		using Umap = std::array<float, cc>;
		typedef std::vector<float> Uvec;
		typedef std::vector<CoreData> Svec;

		static CPU& get();

		uint count();
		static uint cCount();

		void update(Svec& container);

		float refPercent();
		float refPercent(uint core);
		template<typename rep, typename period>
		float intervalPercent(CHRONO::duration<rep, period> interval) {
			this->buffer[0].update();
			std::this_thread::sleep_for(interval);
			update(this->reference);
			return average(this->buffer[0], this->reference[0]);
		}
		template<typename rep, typename period>
		float intervalPercent(uint core, CHRONO::duration<rep, period> interval) {
			if (core > this->c_cores) {
				core = 0;
			}
			this->buffer[core].update();
			std::this_thread::sleep_for(interval);
			update(this->reference);
			return average(this->buffer[core], this->reference[core]);
		}

		Uvec fromReference();
		template<typename rep, typename period>
		Uvec fromInterval(CHRONO::duration<rep, period> interval) {
			update(this->buffer);
			std::this_thread::sleep_for(interval);
			update(this->reference);
			return averageVec(this->buffer, this->reference);
		}

		void average(Svec& first, Svec& second, Uvec& ret);
		Uvec average(Svec& first, Svec& second);

		static float average(CoreData& first, CoreData& second);
		static void averageVec(Svec& first, Svec& second, Uvec& ret);
		static Uvec averageVec(Svec& first, Svec& second);

		static float temp();
		//static float ctemp();

		template<typename rep, typename period>
		static float percent(CHRONO::duration<rep, period> interval) {
			CoreData second, first(0);
			std::this_thread::sleep_for(interval);
			second.update();
			return average(first, second);
		}
		static float percent(int seconds = 1);
	private:
		CPU();
		CPU(const CPU&) = delete;

		uint c_cores;
		Svec reference, buffer;
		static CE_STR source = "/proc/stat";
	};

	class NET {
	private:
		class Interface {
		public:
			enum class Stats {
				RBYTES, RPACKETS, RERR, RDROP,
				RFIFO, FRAME, RCOMPRESSED, MULTICAST,
				TBYTES, TPACKETS, TERR, TDROP, TFIFO,
				COLLS, CARRIER, TCOMPRESSED, TOTAL
			};
			struct MultiStat {
				ulong sent, recv;
			};

			std::string title;
			std::array<ulong, (uint)Stats::TOTAL> data;

			void update();
			bool update(const std::string& id);
			void parseFrom(const std::string& line);

			Interface();
			Interface(const std::string& id);
			Interface(const Interface& other);

			ulong getSent();	//bytes
			ulong getRecv();
			MultiStat getBytes();
			MultiStat getPackets();
			ulong getStat(Stats stat);

		private:
			static const std::array<Stats, 2> s_speeds;	//use a different system if more stats become relevant
			static const std::array<Stats, 2> s_packets;
		};
	public:

	private:
		static CE_STR source = "/proc/net/dev";
	};

	//old
	namespace sys {
		namespace cpu {
			static CE_STR source = "/proc/stat";
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