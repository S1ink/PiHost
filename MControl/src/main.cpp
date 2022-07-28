#include <chrono>
#include <atomic>
#include <iostream>
#include <thread>
#include <array>
#include "pigpio.h"

#define PWM			21
#define FORWARD		20
#define BACKWARD	16

#define STEP_A1		6
#define STEP_A2		13
#define STEP_B1		19
#define STEP_B2		26


namespace stepper {
	size_t
		indexes = 8,
		rotation_steps = 4096
	;
	uint32_t steps =
		0b1001 |
		0b1000 << 4 |
		0b1100 << 8 |
		0b0100 << 12 |
		0b0110 << 16 |
		0b0010 << 20 |
		0b0011 << 24 |
		0b0001 << 28
	;
	// uint32_t steps =
	// 	0b0100 |
	// 	0b0101 << 4 |
	// 	0b0001 << 8 |
	// 	0b1001 << 12 |
	// 	0b1000 << 16 |
	// 	0b1010 << 20 |
	// 	0b0010 << 24 |
	// 	0b0110 << 28
	// ;
}
void setStep(uint8_t si) {
	gpioWrite(STEP_A1, 0b1 & (stepper::steps >> (si * 4 + 3)));
	gpioWrite(STEP_A2, 0b1 & (stepper::steps >> (si * 4 + 2)));
	gpioWrite(STEP_B1, 0b1 & (stepper::steps >> (si * 4 + 1)));
	gpioWrite(STEP_B2, 0b1 & (stepper::steps >> (si * 4)));
}
void setOff() {
	gpioWrite(STEP_A1, 0);
	gpioWrite(STEP_A2, 0);
	gpioWrite(STEP_B1, 0);
	gpioWrite(STEP_B2, 0);
}

void l298n_demo();
void uln2003_demo();

int main() {
	l298n_demo();
	//uln2003_demo();
}



void l298n_demo() {
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

void exitCheck(std::atomic_bool& b) {
	std::cin.ignore();
	b = false;
}
void uln2003_demo() {
	gpioInitialise();
	gpioSetMode(STEP_A1, PI_OUTPUT);
	gpioSetMode(STEP_A2, PI_OUTPUT);
	gpioSetMode(STEP_B1, PI_OUTPUT);
	gpioSetMode(STEP_B2, PI_OUTPUT);

	//std::atomic_bool run = true;
	//std::thread check(exitCheck, std::ref(run));

	int i;
	for(;;) {
		std::cin >> i;
		std::cout << i << std::endl;
		for(size_t s = 0; s < stepper::rotation_steps * ((float)i / 360); s++) {
			setStep(s % stepper::indexes);
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
		setOff();
	}

	gpioTerminate();
}