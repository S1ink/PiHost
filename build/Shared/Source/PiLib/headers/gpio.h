#pragma once

#define INCLUDE_ALL
#include "include.h"
#undef INCLUDE_ALL

namespace pilib {
	int getStatus();
	int getSwitch();
	void activateSwitch(int id);
	void init(float fanspeed = 40.f);
	void setNoctua(float percent);
}