#include "pilib.h"
//#include "app.h"

//static volatile bool run = true;
//util::logger log("/data/logs/rpi_out.txt", std::ios::out | std::ios::app);

//static void input_(void) {
//	while (run) {
//		if (std::cin.get() == 'q') {
//			run = false;
//		}
//	}
//}

//static void callback(int gpio, int level, uint32_t tick) {
//	run = false;
//	log.stampedLog(" : Button pressed - shutdown initialized\n");
//}

constexpr char* cpustat = "/proc/stat";
constexpr std::ios_base::openmode readmode = std::ios::in;

int main(void) {
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	//std::thread itr(input_);
	/*while (run) {
		std::cout << info::elapsed_time(start) << " seconds have elapsed" << newline << newline;
		int wait = (10-(info::elapsed_time(start) - (int)info::elapsed_time(start)))*1000000;
		usleep(wait);
	}*/
	//itr.join();
	std::ifstream reader(cpustat);
	int data[128];
	std::string extras;

	reader >> extras >> data[0] >> data[1] >> data[2];
	std::cout << "DATA: {" << extras << " | " << data[0] << " | " << data[1] << " | " << data[2] << "} END DATA" << newline;
	reader.close();
	std::cout << "Total elapsed time: " << info::elapsed_time(start) << " seconds" << newline;
	return 0;
}