#include <iostream>

#include "resources.h"
#include "timing.h"
#include "unix/stats.h"
#include "sighandle.h"

StopWatch runtime("Runtime", &std::cout, 0);

//void inline wait(uint seconds) {
//	std::this_thread::sleep_for(CHRONO::seconds(seconds));
//}
void inline mwait(uint milliseconds) {
	std::this_thread::sleep_for(CHRONO::milliseconds(milliseconds));
}
//void inline uwait(uint microseconds) {
//	std::this_thread::sleep_for(CHRONO::microseconds(microseconds));
//}
//void inline nwait(uint nanoseconds) {
//	std::this_thread::sleep_for(CHRONO::nanoseconds(nanoseconds));
//}

void on_exit() {
	runtime.end();
}

CE_STR t_red = "\033[38;2;255;0;0m";
CE_STR t_orange = "\033[38;2;255;100;0m";
CE_STR t_yellow = "\033[38;2;240;240;0m";
CE_STR t_green = "\033[38;2;0;255;0m";
CE_STR t_blue = "\033[38;2;50;50;255m";
CE_STR t_purple = "\033[38;2;150;0;255m";
CE_STR t_reset = "\033[0m";

void visualUtilization(float utilization) {
	if (utilization >= 80.f) {
		std::cout << t_red;
	}
	else if (utilization >= 50.f) {
		std::cout << t_orange;
	}
	else if (utilization >= 20.f) {
		std::cout << t_yellow;
	}
	else {
		std::cout << t_green;
	}
}

int main() {
	runtime.setStart();

	CPU& cpustat = CPU::get();

	std::vector<float> utilization;
	float temp;
	utilization.reserve(cpustat.count() + 1);
	utilization.resize(cpustat.count() + 1);

	while (1) {
		cpustat.fromReference(utilization);
		visualUtilization(utilization[0]);
		std::cout << "Total: " << t_reset << utilization[0] << "%\n";
		for (uint i = 1; i < utilization.size(); i++) {
			visualUtilization(utilization[i]);
			std::cout << "Core [" << i << "]: " << t_reset << utilization[i] << "%\n";
		}
		temp = CPU::temp();
		visualUtilization(temp);
		std::cout << "Temp: " << t_reset << temp << "*C\n";
		mwait(500);
		std::cout <<
			"\033[6A"
			"                         \n"
			"                         \n";
		for (uint i = 0; i < cpustat.count(); i++) {
			std::cout << "                         \n";
		}
		std::cout << "\033[6A";
	}

	return 0;
}