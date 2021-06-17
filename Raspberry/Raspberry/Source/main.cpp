#include "pilib.h"
//#include "app.h"

//typedef struct BCM2711_activity {
//	cpustat::UtilizationData total;
//	cpustat::UtilizationData core1;
//	cpustat::UtilizationData core2;
//	cpustat::UtilizationData core3;
//	cpustat::UtilizationData core4;
//};

static volatile bool run = true;

static void input_(void) {
	std::cin.get();
	run = false;
}

int main(int argc, char* argv[]) {
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

	std::thread itr(input_);

	while (run) {
		std::cout << "CPU: {" << info::cputemp() << "*C | " << info::cpu::getPercent() << "%}" << newline;
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