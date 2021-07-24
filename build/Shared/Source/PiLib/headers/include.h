#pragma once

//custom
#include "pigpio.h"
#include "pivar.h"

//io
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <string.h>
#include <sstream>
#include <fstream>
#include <stdexcept>

//timing
#include <ctime>
#include <chrono>
#include <thread>

//memory/types
#include <memory>
#include <atomic>
#include <typeinfo>

//containers
#include <array>
#include <vector>
#include <map>
#include <unordered_map>

//algo
#include <algorithm>
#include <regex>

//os
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>