#pragma once

#ifdef INCLUDE_ALL
#define STD_FULL
#define LIBS
#define VARS
#endif

#ifdef STD_FULL
#define IO
#define TIME
#define TYPE
#define CONTAINERS
#define ALGO
#define OS
#endif

#ifdef LIBS
#include "pigpio.h"
#endif

#ifdef VARS
#include "pivar.h"
#endif

#ifdef IO
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <string.h>
#include <sstream>
#include <fstream>
#include <stdexcept>
#endif

#ifdef TIME
#include <ctime>
#include <chrono>
#include <thread>
#endif

#ifdef TYPE
#include <memory>
#include <atomic>
#include <typeinfo>
#endif

#ifdef CONTAINERS
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#endif

#ifdef ALGO
#include <algorithm>
#include <regex>
#include <cstdarg>
#endif

#ifdef OS
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#endif