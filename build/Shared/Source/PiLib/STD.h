#pragma once

#ifdef STD_FULL
#define IO
#define TIME
#define TYPE
#define CONTAINERS
#define ALGO
#define OS
#endif

#ifndef IOS
#ifndef TIME
#ifndef TYPE
#ifndef CONTAINERS
#ifndef ALGO
#ifndef OS
#define IOS
#define TIME
#define TYPE
#define CONTAINERS
#define ALGO
#define OS
#endif
#endif
#endif
#endif
#endif
#endif

#ifdef IOS
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <string.h>
#include <sstream>
#include <fstream>
#include <stdexcept>
#undef IO
#endif

#ifdef TIME
#include <ctime>
#include <chrono>
#include <thread>
#undef TIME
#endif

#ifdef TYPE
#include <memory>
#include <atomic>
#include <typeinfo>
#undef TYPE
#endif

#ifdef CONTAINERS
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#undef CONTAINERS
#endif

#ifdef ALGO
#include <algorithm>
#include <regex>
#include <cstdarg>
#undef ALGO
#endif

#ifdef OS
#include <unistd.h>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/time.h>
#undef OS
#endif