//static volatile bool run = true;
//util::logger log("/data/logs/rpi_out.txt", std::ios::out | std::ios::app);
//
//-----
//
//static void input_(void) {
//	while (run) {
//		if (std::cin.get() == 'q') {
//			run = false;
//		}
//	}
//}
//
//-----
//
//static void callback(int gpio, int level, uint32_t tick) {
//	run = false;
//	log.stampedLog(" : Button pressed - shutdown initialized\n");
//}
//
//-----
//
//std::thread itr(input_);
//while (run) {
//	std::cout << info::elapsed_time(start) << " seconds have elapsed" << newline << newline;
//	int wait = (10 - (info::elapsed_time(start) - (int)info::elapsed_time(start))) * 1000000;
//	usleep(wait);
//}
//itr.join();
//
//-----
//
//static void demoFan() {
//	int iterate = 2;
//	float percent = 0;
//	int delta = 5;
//	while (run) {
//		if ((percent > 100) | (percent < 0)) {
//			delta *= -1;
//			percent += delta;
//		}
//		gpio::setNoctua(percent);
//		std::cout << "Fan set to: " << percent << "% speed" << newline;
//		sleep(iterate);
//		percent += delta;
//	}
//}