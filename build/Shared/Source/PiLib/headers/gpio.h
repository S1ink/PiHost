#pragma once

#include "include.h"

namespace pilib {
	int getStatus();
	int getSwitch();
	void activateSwitch(int id);
	void init(float fanspeed = 40.f);
	void setNoctua(float percent);
}