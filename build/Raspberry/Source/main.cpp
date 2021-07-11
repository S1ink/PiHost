#include "pilib.h"

std::atomic_bool run = { true };

static void endCondition() {
	std::cin.ignore();
	run = false;
}

void work(std::ostream& output) {
	output << info::dateStamp() << ": work done?" << newline;
}

//void t1(void) {
//	std::cout << "test function 1" << newline;
//}
//
//void t2(void) {
//	std::cout << "test function 2" << newline;
//}
//
//void t3(void) {
//	std::cout << "test function 3" << newline;
//}
//
//typedef void(*basicfunc)(void);

int main(int argc, char* argv[]) {
	timing::StopWatch runtime;
	
	std::thread looping(threading::loopingThread<CHRONO::seconds::rep, CHRONO::seconds::period, void, std::ostream const&, threading::templatefunc>, std::ref(run), CHRONO::seconds(5), CHRONO::seconds(10), threading::streamWrapper, std::ofstream("/data/logs/work.txt", std::ios::app), work);
	endCondition();
	looping.join();

	/*std::map<std::string, basicfunc> funcmap;

	funcmap.insert(std::pair<std::string, basicfunc>("test1", t1));
	funcmap.insert(std::pair<std::string, basicfunc>("test2", t2));
	funcmap.insert(std::pair<std::string, basicfunc>("test3", t3));

	std::string input;
	while (true) {
		break;
		std::cout << "Enter a function to test: ";
		std::cin >> input;
		if (input == "exit") {
			break;
		}
		auto result = funcmap.find(input);
		if (result == funcmap.end()) {
			std::cout << "Function not found" << newline;
		}
		else {
			result->second();
		}
	}*/

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