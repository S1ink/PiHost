#pragma once

//#define PILIB_FULL

#ifdef INCLUDE_STD
#include "PiLib/STD.h"
#endif

#ifdef LEGACY
#ifdef PILIB_FULL
#undef PILIB_FULL
#endif
#include "PiLib/Legacy/pilib.h"
#define VARS
#endif

#ifdef PILIB_FULL
#define IO
#define NETWORK
#define RUNTIME
#define SYSTEM
#define UTILITY
#define VARS
#endif

#ifndef IO
#ifndef NETWORK
#ifndef RUNTIME
#ifndef SYSTEM
#ifndef UTILITY
#define IO
#define NETWORK
#define RUNTIME
#define SYS
#define UTILITY
#endif
#endif
#endif
#endif
#endif


#ifdef IO
#include "PiLib/IO.h"
#endif

#ifdef NETWORK
#include "PiLib/Network.h"
#endif

#ifdef RUNTIME
#include "PiLib/Runtime.h"
#endif

#ifdef SYS
#include "PiLib/System.h"
#endif

#ifdef UTILITY
#include "PiLib/Utility.h"
#endif

#ifdef VARS
#include "PiLib/pivar.h"
#endif