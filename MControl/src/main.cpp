#include <chrono>
#include <atomic>
#include <iostream>
#include <thread>
#include "pigpio.h"

#define PWM			21
#define FORWARD		20
#define BACKWARD	16


int main() {
	gpioInitialise();
	gpioSetMode(PWM, PI_OUTPUT);
	gpioSetMode(FORWARD, PI_OUTPUT);
	gpioSetMode(BACKWARD, PI_OUTPUT);

	gpioWrite(FORWARD, PI_HIGH);
	gpioWrite(BACKWARD, PI_LOW);
	gpioPWM(PWM, 127);

	std::cout << "Forward@50%\nValid Options: f[orward], b[ackward], s[top], r[un], 0-100[%] e[xit]" << std::endl;

	char i;
	for(;;) {
		std::cout << "\nEnter adjustment: ";
		i = std::cin.get();
		if(i >= '0' && i <= '9') {
			std::cin.unget();
			uint16_t n;
			std::cin >> n;
			gpioPWM(PWM, (float)n / 100 * 255);
			std::cout << "Set PWM to " << n << "%\n";
		} else {
			switch(i) {
				case 'f': {
					gpioWrite(FORWARD, PI_HIGH);
					gpioWrite(BACKWARD, PI_LOW);
					std::cout << "Forward\n";
					break;
				}
				case 'b': {
					gpioWrite(FORWARD, PI_LOW);
					gpioWrite(BACKWARD, PI_HIGH);
					std::cout << "Backward\n";
					break;
				}
				case 's': {
					gpioWrite(FORWARD, PI_LOW);
					gpioWrite(BACKWARD, PI_LOW);
					std::cout << "Stop\n";
					break;
				}
				case 'r': {
					gpioWrite(FORWARD, PI_HIGH);
					gpioWrite(BACKWARD, PI_LOW);
					std::cout << "Run\n";
					break;
				}
				case 'e': {
					std::cout << "Exit\n";
					gpioWrite(FORWARD, PI_LOW);
					gpioWrite(BACKWARD, PI_LOW);
					gpioWrite(PWM, PI_LOW);
					gpioTerminate();
					return 0;
				}
				case '\r':
				case '\n':
				{
					break;
				}
				default: {
					std::cout << "Invalid option: " << i << std::endl;
				}
			}
		}
	}
}