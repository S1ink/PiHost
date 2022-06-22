#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <atomic>
#include <pigpio.h>

#include "stats2.h"

#define GREEN	23
#define RED		24

#define T_LOW	30	// *C
#define T_MAX	100	// *C


std::atomic_bool is_exit;

void terminate() {
	is_exit = true;
	//std::this_thread::sleep_for(std::chrono::milliseconds(20));
	gpioWrite(GREEN, 0);
	gpioWrite(RED, 0);
	std::cout << "Exit." << std::endl;
	gpioTerminate();
}



int main() {
	gpioInitialise();
	gpioSetMode(GREEN, PI_OUTPUT);
	gpioSetMode(RED, PI_OUTPUT);

	CoreStats statistics;

	//float t;
	float u = 0.f, ul;
	for(;!is_exit;) {
		// t = (CoreStats::temp() - T_LOW) / (T_MAX - T_LOW);
		// t = t > 1.f ? 1 : t < 0 ? 0 : t;
		ul = u;
		u = statistics.fromLast();
		//std::cout << u * 100.f << std::endl;
		for(size_t i = 0; i < 50; i++) {
			ul += (u - ul) / 50;
			gpioPWM(GREEN, 255 - ul * 255);
			gpioPWM(RED, 255 * ul * ul * ul);
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}

	atexit(terminate);
}