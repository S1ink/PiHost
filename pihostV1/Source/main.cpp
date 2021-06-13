#include "pilib.h"
//#include "app.h"

static volatile bool run = true;
util::logger log("/data/logs/rpi_out.txt", std::ios::out | std::ios::app);

//static void input_(void) {
//	while (run) {
//		if (std::cin.get() == 'q') {
//			run = false;
//		}
//	}
//}

static void callback(int gpio, int level, uint32_t tick) {
	run = false;
	log.stampedLog(" : Button pressed - shutdown initialized\n");
}

int main(void) {
	//std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	log.stampedLog(" : Startup\n");
	gpio::init();
	gpioSetISRFunc(p_switch, RISING_EDGE, 0, callback);
	//std::thread itr(input_);
	double temps[2];
	while (run) {
		temps[0] = info::cputemp();
		temps[1] = info::gputemp();
		if (temps[0] >= 40) {
			log.stampedLog(" - Placeholder temp warning\n");
		}if (temps[1] >= 40) {
			log.stampedLog(" - Placeholder temp warning\n");
		}
		//std::cout << info::elapsed_time(start) << " seconds have elapsed" << newline << newline;
		//int wait = (10-(info::elapsed_time(start) - (int)info::elapsed_time(start)))*1000000;
		//usleep(wait);
		sleep(10);
	}
	//itr.join();
	gpioTerminate();
	log.stampedLog(" : All processes ended - shutting down\n\n");
	system("sudo halt");
	//std::cout << "Total elapsed time: " << info::elapsed_time(start) << " seconds" << newline;
	return 0;
}