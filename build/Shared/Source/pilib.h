#pragma once

//#define PILIB_FULL

#ifdef STD_FULL
#include "PiLib/headers/include.h"
#endif

#ifdef PILIB_FULL
#define INFO
#define UTIL
#define TIMING
#define FILES
#define THREADING
#define NETWORKING
#define GPIO
#define VARS
#endif

#ifdef INFO
#include "PiLib/headers/info.h"
#endif

#ifdef UTIL
#include "PiLib/headers/utility.h"
#endif

#ifdef TIMING
#include "PiLib/headers/timing.h"
#endif

#ifdef FILES
#include "PiLib/headers/files.h"
#endif

#ifdef THREADING
#include "PiLib/headers/threading.h"
#endif

#ifdef NETWORKING
#include "PiLib/headers/networking.h"
#endif

#ifdef GPIO
#include "PiLib/headers/gpio.h"
#endif

#ifdef VARS
#include "PiLib/headers/pivar.h"
#endif