#include "pilib.h"

//typedef struct BCM2711_activity {
//	cpustat::UtilizationData total;
//	cpustat::UtilizationData core1;
//	cpustat::UtilizationData core2;
//	cpustat::UtilizationData core3;
//	cpustat::UtilizationData core4;
//};

static volatile bool run = true;

static void endCondition(void) {
	std::cin.get();
	run = false;
}

int main(int argc, char* argv[]) {
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	//gpio::init();
	//std::thread end(endCondition);
	//std::thread fan(demoFan);

	/*std::chrono::time_point<std::chrono::system_clock> stopwatch = std::chrono::system_clock::now();
	info::cpu::getPercent();
	std::cout << "CPU ACTIVITY FUNCTION TIME: " << info::elapsed_time(stopwatch) << newline;

	stopwatch = std::chrono::system_clock::now();
	info::cputemp();
	std::cout << "CPU TEMP FUNCTION TIME: " << info::elapsed_time(stopwatch) << newline;

	stopwatch = std::chrono::system_clock::now();
	gpioInitialise();
	std::cout << "GPIO INITIALIZATION FUNCTION TIME: " << info::elapsed_time(stopwatch) << newline;

	stopwatch = std::chrono::system_clock::now();
	gpio::setNoctua(50);
	std::cout << "SET FAN FUNCTION TIME: " << info::elapsed_time(stopwatch) << newline;

	stopwatch = std::chrono::system_clock::now();
	compute();
	std::cout << "BASIC COMPUTATION TIME: " << info::elapsed_time(stopwatch) << newline;*/

	//util::csv::Csv example = util::csv::csvRead("/data/pihost/example.csv");
	//std::cout << (example)[0][1] << newline;

	//std::cout << util::rclone("/data/pihost/tasks.txt", "/data/") << newline;
	
	//std::cout << util::rclone("/data/pihost/tasks.txt", "/data/") << newline;
	//util::s_rclone("/data/pihost/tasks.txt", "/data/");

	//files::csv::csvSync("/data/pihost/backups.csv");
	//util::rsync("/data/pihost/tasks.txt", "/data/");

	std::string path = "/data/pihost/rsyncbackup.csv";
	//files::csv::Csv data;
	//data.reserve(5);

	//files::csv::csvRead(path.c_str(), data);

	//std::cout << files::csv::winCheck(path.c_str()) << newline;

	//for (unsigned int i = 0; i < data.size(); i++) {
	//	//std::cout << i << newline;
	//	//std::cout << (data[i]).size() << newline;
	//	for (unsigned int j = 0; j < data[i].size(); j++) {
	//		//std::cout << j;
	//		std::cout << "line " << i << ", section " << j << " - " << data[i][j] << newline;
	//	}
	//}
	std::ofstream output("/data/logs/testrun.txt");
	files::csv::winSync(path.c_str(), output);

	while (run) {
		//std::cout << "CPU: {" << info::cputemp() << "*C | " << info::cpu::getPercent() << "%}" << newline;
		break;
	}

	//end.join();
	//fan.join();
	//gpio::setNoctua(50);
	//gpioTerminate();
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