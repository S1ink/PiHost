//IMPORTANT VARIABLES TO BE INCLUDED WHERE NECESSARY
#pragma once

#define CHRONO std::chrono
#define CE_STRING constexpr char const*

constexpr char newline = 10;	// ('/n')
constexpr char space = 32;		// (' ')
constexpr char csvd = 44;		// (',')

namespace locations {
	namespace stats {
		CE_STRING cpu = "/proc/stat";
		CE_STRING network = "/proc/net/dev";
		CE_STRING load = "/proc/loadavg";
		CE_STRING disk = "/proc/diskstats";
		CE_STRING mem = "/proc/meminfo";
	}
}

namespace gpin {
	constexpr int pi_fan = 18;
	constexpr int pi_power = 3;

	constexpr int pc_power = 20;
	constexpr int pc_reset = 16;
	constexpr int pc_status = 12;
}