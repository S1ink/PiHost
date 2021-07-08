#include "pilib.h"

//typedef struct BCM2711_activity {
//	cpustat::UtilizationData total;
//	cpustat::UtilizationData core1;
//	cpustat::UtilizationData core2;
//	cpustat::UtilizationData core3;
//	cpustat::UtilizationData core4;
//};

volatile bool run = true;
//volatile bool ctrl = true;

static void endCondition() {
	std::cin.get();
	run = false;
}

//void func(int a, int b) {
//	std::cout << info::dateStamp() << newline << "a + b = " << a + b << newline;
//	ctrl = false;
//	return;
//}

void work() {
	std::cout << info::dateStamp() << ": work done?" << newline;
}

int main(int argc, char* argv[]) {
	timing::StopWatch runtime;
	
	/*std::thread timed(timing::routineThread<void>, std::ref(run), timing::DayTime(17, 55, 0), work);
	endCondition();
	timed.join();*/

	//std::cout << "updates: " << util::aptUpdates(std::cout) << newline;
	//std::cout << "upgrades: " << util::aptUpgrade(std::cout) << newline;

	/*int num1;
	int num2;
	int num3;

	std::string numbuff, numbers = "10,-157,83989\n";
	std::istringstream line(numbers);

	std::getline(line, numbuff, csvd);
	{
		std::istringstream num(numbuff);
		num >> num1;
	}
	std::getline(line, numbuff, csvd);
	{
		std::istringstream num(numbuff);
		num >> num2;
	}
	std::getline(line, numbuff, csvd);
	{
		std::istringstream num(numbuff);
		num >> num3;
	}

	std::cout << num1 << newline;
	std::cout << num2 << newline;
	std::cout << num3 << newline;*/

	files::csv::parseTasks("/data/pihost/tasks.csv", std::cout);

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