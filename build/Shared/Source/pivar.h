//IMPORTANT VARIABLES TO BE INCLUDED WHERE NECESSARY
#pragma once

#define CHRONO std::chrono
#define CE_STR constexpr char const*

constexpr char newline = 10;	// ('\n')
constexpr char space = 32;		// (' ')
constexpr char csvd = 44;		// (',')
constexpr char cr = 13;			// ('\r')
constexpr char null = 0;		// ('\0')

namespace locations {
	namespace stats {
		CE_STR cpu = "/proc/stat";
		CE_STR network = "/proc/net/dev";
		CE_STR load = "/proc/loadavg";
		CE_STR disk = "/proc/diskstats";
		CE_STR mem = "/proc/meminfo";
	}

	namespace external {

		CE_STR winbackup = "/data/pihost/Live/external/rsyncbackup.csv";
		CE_STR r_winbackup = "../external/rsyncbackup.csv";
		CE_STR tasks = "/data/pihost/Live/external/tasks.csv";
		CE_STR r_tasks = "../external/tasks.csv";
	}
}

namespace gpin {
	constexpr unsigned short pi_fan = 18;
	constexpr unsigned short pi_power = 3;

	constexpr unsigned short pc_power = 20;
	constexpr unsigned short pc_reset = 16;
	constexpr unsigned short pc_status = 12;
}