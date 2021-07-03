//IMPORTANT VARIABLES TO BE INCLUDED WHERE NECESSARY

#pragma once

constexpr char newline = 10;	// ('/n')
constexpr char space = 32;		// (' ')
constexpr char csvd = 44;		// (',')

namespace locations {
	namespace stats {
		constexpr char* cpu = "/proc/stat";
		constexpr char* network = "/proc/net/dev";
		constexpr char* load = "/proc/loadavg";
		constexpr char* disk = "/proc/diskstats";
		constexpr char* mem = "/proc/meminfo";
	}
}

namespace gpin {
	constexpr int pi_fan = 18;
	constexpr int pi_power = 3;

	constexpr int pc_power = 20;
	constexpr int pc_reset = 16;
	constexpr int pc_status = 12;
}