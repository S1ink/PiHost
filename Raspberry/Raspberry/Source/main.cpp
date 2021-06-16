#include "pilib.h"
//#include "app.h"

namespace stat {
	constexpr char* cpu = "/proc/stat";
	constexpr char* network = "/proc/net/dev";

	constexpr char* load = "/proc/loadavg";
	constexpr char* disk = "/proc/diskstats";
	constexpr char* mem = "/proc/meminfo";
}

namespace var {
	constexpr int usr_hz_psec = 100;
	constexpr int cpu_states = 10;
}

// * * * * 

namespace cpustat {
	constexpr char* srcpath = "/proc/stat";
	constexpr int parsable_states = 10;

	struct ReadLine {
		std::string title;
		size_t usr_hz[parsable_states];
	};

	struct UtilizationData {
		std::string identifier;
		float activity;
	};

	void read_(std::vector<cpustat::ReadLine>& container) {
		std::ifstream reader(srcpath);
		std::string linebuffer;

		while (std::getline(reader, linebuffer)) {
			if (!linebuffer.compare(0, 3, "cpu")) {
				std::istringstream stream(linebuffer);

				container.emplace_back(ReadLine());
				ReadLine& line = container.back();

				stream >> line.title;
				//modify titles here
				for (int i = 0; i < parsable_states; i++) {
					stream >> line.usr_hz[1];
				}
			}
		}
		reader.close();
	}

	size_t netIdle(const ReadLine& dataline) {
		return dataline.usr_hz[3] + dataline.usr_hz[4];
	}

	size_t netActive(const ReadLine& dataline) {
		return dataline.usr_hz[0] + dataline.usr_hz[1] + dataline.usr_hz[2] + dataline.usr_hz[5] + dataline.usr_hz[6] + dataline.usr_hz[7] + dataline.usr_hz[8] + dataline.usr_hz[9];
	}

	std::vector<cpustat::UtilizationData> convertVectorData(std::vector<ReadLine>& snapshot1, std::vector<ReadLine>& snapshot2) {
		std::vector<cpustat::UtilizationData> data;
		data.reserve(5);

		float activetime;
		float idletime;
		float totaltime;

		for (size_t i = 0; i < snapshot1.size(); i++) {
			data.emplace_back(UtilizationData());
			UtilizationData entry = data.back();

			entry.identifier = snapshot1[i].title;

			activetime = static_cast<float>(netActive(snapshot2[i]) - netActive(snapshot1[i]));
			idletime = static_cast<float>(netIdle(snapshot2[i]) - netIdle(snapshot1[i]));
			totaltime = activetime + idletime;

			entry.activity = 100.f * (activetime / totaltime);
		}
		return data;
	}

}

// * * * *

//typedef struct CPURaw {
//	std::string title;
//	size_t lengths[var::cpu_states];
//};
//
//typedef struct cpuratio {
//	size_t wait;
//	size_t util;
//};
//
//typedef struct BCM2711_u {
//	size_t cpu;
//	size_t core1;
//	size_t core2;
//	size_t core3;
//	size_t core4;
//};
//
////for 4-core cpu
//void read4cpulines_t(std::vector<cpuraw>& container) {
//	std::ifstream reader(stat::cpu);
//	
//	for (int i = 0; i < 5; i++) {
//		container.emplace_back(cpuraw());
//		cpuraw& line = container.back();
//
//		reader >> line.cpu;
//
//		for (int i = 0; i < var::cpu_states; i++) {
//			reader >> line.lengths[i];
//		}
//	}
//	reader.close();
//}
//
//void readcpuTotalOnly_t(cpuraw& container) {
//	std::ifstream reader(stat::cpu);
//
//	reader >> container.cpu;
//	for (int i = 0; i < var::cpu_states; i++) {
//		reader >> container.lengths[i];
//	}
//	reader.close();
//}
//
//cpuratio cpuRatio(const cpuraw& line) {
//	cpuratio ratio;
//
//	ratio.wait = line.lengths[3] + line.lengths[4];
//	ratio.util = line.lengths[0] + line.lengths[1] + line.lengths[2] + line.lengths[5] + line.lengths[6] + line.lengths[7] + line.lengths[8] + line.lengths[9];
//
//	return ratio;
//}
//
//float deltaUtilization(const cpuratio& snap1, const cpuratio& snap2) {
//	const float active = static_cast<float>(snap2.util - snap1.util);
//	const float idle = static_cast<float>(snap2.wait - snap1.wait);
//	const float total = active + idle;
//	return 100.f * (active / total);
//}

static volatile bool run = true;

static void input_(void) {
	while (run) {
		if (std::cin.get() == 'q') {
			run = false;
		}
	}
}

int main(int argc, char* argv[]) {
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

	std::thread itr(input_);
	
	std::vector<cpustat::ReadLine> snap1;
	std::vector<cpustat::ReadLine> snap2;
	std::vector<cpustat::UtilizationData> output;

	while (run) {
		cpustat::read_(snap1);
		sleep(1);
		cpustat::read_(snap2);

		output = cpustat::convertVectorData(snap1, snap2);
		for (size_t i = 0; i < output.size(); i++) {
			std::cout << "[" << output[i].identifier << "]: " << output[i].activity << "%" << newline;
		}
	}

	itr.join();

	std::cout << "Total elapsed time: " << info::elapsed_time(start) << " seconds" << newline;
	return 0;
}

//NOTES
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