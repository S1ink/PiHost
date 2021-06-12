#include "pilib.h"
//#include "app.h"

static volatile bool run = true;

static void input_(void) {
	while (run) {
		if (std::cin.get() == 'q') {
			run = false;
		}
	}
}

static void callback(int gpio, int level, uint32_t tick) {
	gpio::activateSwitch(p_power);
	std::cout << newline << " * * * BUTTON PRESSED * * * " << newline;
}

int main(void) {
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	gpio::init();
	gpioSetISRFunc(p_switch, RISING_EDGE, 0, callback);
	std::thread itr(input_);
	std::cout << newline << " * * * Loop started, enter 'q' to exit * * * " << newline << newline;
	while (run) {
		std::cout << info::dateStamp() << " - CPU temp: " << info::cputemp() << newline;
		std::cout << info::dateStamp() << " - GPU temp: " << info::gputemp() << newline;
		std::cout << "gpio pc status: " << gpio::getStatus() << newline;
		std::cout << "gpio button status: " << gpio::getSwitch() << newline;
		std::cout << info::elapsed_time(start) << " seconds have elapsed" << newline << newline;
		int wait = (1-(info::elapsed_time(start) - (int)info::elapsed_time(start)))*1000000;
		std::cout << wait << newline << newline;
		usleep(wait);
	}
	itr.join();
	gpioTerminate();
	//gpioDebugMode();
	std::cout << "Total elapsed time: " << info::elapsed_time(start) << " seconds" << newline;
	return 0;
}