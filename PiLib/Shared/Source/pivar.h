//IMPORTANT VARIABLES TO BE INCLUDED WHERE NECESSARY

#pragma once

constexpr char newline = '\n';
constexpr char csvsep = ',';

namespace locations {
	namespace stats {
		constexpr char* cpu = "/proc/stat";
		constexpr char* network = "/proc/net/dev";
		constexpr char* load = "/proc/loadavg";
		constexpr char* disk = "/proc/diskstats";
		constexpr char* mem = "/proc/meminfo";
	}
}