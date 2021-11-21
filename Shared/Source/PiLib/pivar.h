//GLOBAL COMPILE-TIME VARS
#pragma once

#ifndef _RESOURCES
#define _RESOURCES

#define CHRONO std::chrono
#define CE_STR constexpr char const*

constexpr char newline = 10;	// ('\n')
constexpr char space = 32;		// (' ')
constexpr char comma = 44;		// (',')
constexpr char cr = 13;			// ('\r' or carriage-return)
constexpr char null = 0;		// ('\0')
constexpr char s_dir =
#ifdef _WIN32
92;                             //('\')
#else //if '__unix__' or '__linux__' or '__APPLE__' 
47;                             //('/')
#endif

CE_STR endline =
#ifdef _WIN32
"\r\n";
#else 
"\n";
#endif

#endif

//namespace locations {
//	namespace stats {
//		CE_STR cpu = "/proc/stat";
//		CE_STR network = "/proc/net/dev";
//		CE_STR load = "/proc/loadavg";
//		CE_STR disk = "/proc/diskstats";
//		CE_STR mem = "/proc/meminfo";
//	}
//
//	namespace external { 
//		CE_STR winbackup = "/data/pihost/resources/rsyncbackup.csv";
//		//CE_STR r_winbackup = "../external/rsyncbackup.csv";
//		CE_STR tasks = "/data/pihost/resources/tasks.csv";
//		//CE_STR r_tasks = "../external/tasks.csv";
//	}
//}