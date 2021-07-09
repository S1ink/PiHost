#include "pilib.h"

//typedef struct BCM2711_activity {
//	cpustat::UtilizationData total;
//	cpustat::UtilizationData core1;
//	cpustat::UtilizationData core2;
//	cpustat::UtilizationData core3;
//	cpustat::UtilizationData core4;
//};

std::atomic_bool run = { true };
//volatile bool ctrl = true;

static void endCondition() {
	std::cin.ignore();
	run = false;
}

//void func(int a, int b) {
//	std::cout << info::dateStamp() << newline << "a + b = " << a + b << newline;
//	ctrl = false;
//	return;
//}

void work(std::ofstream const& output) {
	std::ofstream& out = const_cast<std::ofstream&>(output);
	out << info::dateStamp() << ": work done?" << newline;
	out.close();
}

int main(int argc, char* argv[]) {
	timing::StopWatch runtime;
	std::ofstream out("/data/logs/work.txt", std::ios::app);
	
	std::thread timed(timing::routineThread<void, std::ofstream const&>, std::ref(run), timing::DayTime(22, 17, 00), work, std::ofstream("/data/logs/work.txt", std::ios::app));
	endCondition();
	timed.join();

	runtime.pTotal();
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