#pragma once

#include "pilib.h"

void gpio1();
void gpio2();
int gpioDebugMode(bool dinit = false);

namespace db {
	void ptitle();
	int _input(std::string desc = "Type your selection: ");
	void pmodes();
	void ePins();
	void vPins();
	void ePWM();
	void sMacros();
}
