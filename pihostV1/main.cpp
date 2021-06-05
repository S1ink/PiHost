#include "pilib.h"

int main(void) {
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	std::cout << sysinfo::dateStamp() << " - CPU temp: " << sysinfo::cputemp() << newline;
	std::cout << sysinfo::dateStamp() << " - GPU temp: " << sysinfo::gputemp() << newline;
	std::cout << sysinfo::elapsed_time(start) << " seconds have elapsed" << newline;
	std::cout << util::exec("/usr/bin/rclone sync /data --include /pihost/** Mega-backups:Backups/ -vv") << newline;
	std::cout << sysinfo::elapsed_time(start) << " seconds have elapsed" << newline;
	std::cout << sysinfo::dateStamp() << " - CPU temp: " << sysinfo::cputemp() << newline;
	std::cout << sysinfo::dateStamp() << " - GPU temp: " << sysinfo::gputemp() << newline;
	return 0;
}