#include <iostream>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <chrono>
#include <sstream>
#include <atomic>

#include "pigpio.h"

#define CHRONO std::chrono
#define CE_STRING constexpr char const*

CE_STRING version = "1.0.1";

constexpr char newline = 10;	// ('\n')
constexpr char space = 32;		// (' ')
constexpr char csvd = 44;		// (',')
constexpr char cr = 13;			// ('\r')
constexpr char null = 0;		// ('\0')

constexpr int p_fan = 18;
constexpr int p_switch = 3;

//* * * * * * * * * * * *

class StopWatch {
    CHRONO::time_point<CHRONO::high_resolution_clock> start;
public:
    StopWatch(bool now = true) {
        if (now) {
            start = CHRONO::high_resolution_clock::now();
        }
    }
    void setStart() {
        start = CHRONO::high_resolution_clock::now();
    }
    double getDuration() {
        CHRONO::duration<double> diff = CHRONO::high_resolution_clock::now() - start;
        return diff.count();
    }
    void pTotal() {
        CHRONO::duration<double> diff = CHRONO::high_resolution_clock::now() - start;
        std::cout << "Total elapsed time: " << diff.count() << " seconds" << newline;
    }
};

float cpuTemp() {
    float systemp;
    FILE* thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    fscanf(thermal, "%f", &systemp);
    fclose(thermal);
    systemp /= 1000;
    return systemp;
}

//float gpuTemp() {
//    char var[12];
//    FILE* pipe = popen("vcgencmd measure_temp", "r");
//    fscanf(pipe, "%s", var);
//    float temp = (((float)var[5] - 48) * 10 + ((float)var[6] - 48) + ((float)var[8] - 48) / 10);
//    pclose(pipe);
//    return temp;
//}

struct LineParse {
    std::string title;
    size_t time[10];
};

size_t getIdle(const LineParse& data) {
    return data.time[3] + data.time[4];
}

size_t getActive(const LineParse& data) {
    return data.time[0] + data.time[1] + data.time[2] + data.time[5] + data.time[6] + data.time[7] + data.time[8] + data.time[9];
}

void readMain(LineParse& container) {
    std::ifstream reader("/proc/stat");

    reader >> container.title;
    for (int i = 0; i < 10; i++) {
        reader >> container.time[i];
    }
    reader.close();
}

float getPercent(unsigned int wait = 1) {
    LineParse time1, time2;
    readMain(time1);
    sleep(wait);
    readMain(time2);
    float active = static_cast<float>(getActive(time2) - getActive(time1));
    float total = active + (static_cast<float>(getIdle(time2) - getIdle(time1)));
    return 100.f * (active / total);
}

char* dateStamp() {
    time_t now = CHRONO::system_clock::to_time_t(CHRONO::system_clock::now());
    char* t = ctime(&now);
    t[strlen(t) - 1] = '\0';
    return t;
}

unsigned char clearEnd(std::string& str) {
    unsigned char ret = null;
    unsigned int len = str.length();
    for (unsigned char i = 1; i < 3; i++) {
        if (str[len - i] == 13 || str[len - i] == 10) {
            ret = str[len - i];
            str.pop_back();
        }
    }
    return ret;
}

std::string exec(const char* command) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        result.append("popen function failed");
        pclose(pipe);
        return result;
    }
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL) {
            result.append(buffer);
        }
    }
    pclose(pipe);
    return result;
}

int aptUpdate(std::ostream& out) {
    int result = 0;
    char buffer[128];
    std::string line = "";
    FILE* pipe = popen("sudo apt update", "r");
    if (!pipe) {
        result = -1;
    }
    else {
        bool found = false;
        while (fgets(buffer, 128, pipe)) {
            out << buffer;
            if (!found) {
                line.append(buffer);
                if (std::stringstream(line) >> result) {
                    break;
                }
                else {
                    line = "";
                }
            }
        }
    }
    pclose(pipe);
    return result;
}

int aptUpgrade(std::ostream& out) {
    int result = 0;
    char buffer[128];
    std::string line = "";
    FILE* pipe = popen("sudo apt-get upgrade -y", "r");
    if (!pipe) {
        result = -1;
    }
    else {
        bool found = false;
        while (fgets(buffer, 128, pipe)) {
            out << buffer;
            if (!found) {
                line.append(buffer);
                if (std::stringstream(line) >> result) {
                    break;
                }
                else {
                    line = "";
                }
            }
        }
    }
    pclose(pipe);
    return result;
}

void aptRun(std::ofstream const& output) {
    StopWatch ptime;
    std::ofstream& out = const_cast<std::ofstream&>(output);
    out << "Pihost internal APT updater initialized. (" << dateStamp() << ")" << newline << newline;
    int updates = aptUpdate(out);
    out << newline << newline << " * * * * * * * * " << newline << newline;
    int upgrades = aptUpgrade(out);
    out << "Process finished at: " << dateStamp() << newline << "Total elapsed time: " << ptime.getDuration() << " seconds." << newline;
    out << "Updates: " << updates << newline << "Upgrades: " << upgrades;
}

struct WinSync {
    std::string name;
    std::string source;
    std::string destination;
    std::string options;
};

std::string rsync(const char* source, const char* destination, const char* options = "-va") {
    std::stringstream command;
    char buffer[128];
    std::string output = "";
    command << "sudo rsync " << options << space << source << space << destination;
    FILE* pipe = popen(command.str().c_str(), "r");
    if (!pipe) {
        output.append("popen function failed");
        pclose(pipe);
        return output;
    }
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL) {
            output.append(buffer);
        }
    }
    pclose(pipe);
    return output;
}

//std::string rclone(const char* source, const char* destination, const char* mode = "sync") {
//    std::stringstream command;
//    char buffer[128];
//    std::string output = "";
//    command << "rclone " << mode << space << source << space << destination << " -vv";
//    FILE* pipe = popen(command.str().c_str(), "r");
//    if (!pipe) {
//        output.append("popen function failed");
//        pclose(pipe);
//        return output;
//    }
//    while (!feof(pipe)) {
//        if (fgets(buffer, 128, pipe) != NULL) {
//            output.append(buffer);
//        }
//    }
//    pclose(pipe);
//    return output;
//}

void winSync(const char* filepath, std::ostream& output) {
    std::ifstream reader(filepath);
    std::string linebuffer;
    WinSync databuffer;

    std::getline(reader, linebuffer);
    if (!(linebuffer == "name,source,destination,options")) {
        output << "Instruction file is not in the correct format.\n";
    }
    else {
        while (std::getline(reader, linebuffer)) {
            std::istringstream linestream(linebuffer);
            std::getline(linestream, databuffer.name, ',');
            std::getline(linestream, databuffer.source, ',');
            std::getline(linestream, databuffer.destination, ',');
            if (std::getline(linestream, databuffer.options)) {
                output << dateStamp() << " - Started syncing {" << databuffer.name << "}...\n* * * * * * * * * * *\n" << rsync(databuffer.source.c_str(), databuffer.destination.c_str(), databuffer.options.c_str()) << "* * * * * * * * * *\n\n\n";
            }
            else {
                output << dateStamp() << " - Started syncing {" << databuffer.name << "}...\n* * * * * * * * * * *\n" << rsync(databuffer.source.c_str(), databuffer.destination.c_str()) << "* * * * * * * * * *\n\n\n";
            }
        }
    }
    reader.close();
    return;
}

void winBackup(std::ofstream const& output) {
    StopWatch ptime;
    std::ofstream& out = const_cast<std::ofstream&>(output);
    out << "Pihost internal winbackup initialized. (" << dateStamp() << ")" << newline << newline;
    winSync("/data/pihost/rsyncbackup.csv", out);
    out << "Process finished at: " << dateStamp() << newline << "Total elapsed time: " << ptime.getDuration() << " seconds.";
    out.close();
}

struct DayTime {
    uint8_t hr;
    uint8_t min;
    uint8_t sec;
public:
    DayTime(const uint8_t hr, const uint8_t min, const uint8_t sec) : hr(hr), min(min), sec(sec) {}
};

time_t d_untilNext(const DayTime tme) {
    CHRONO::time_point<CHRONO::system_clock> now = CHRONO::system_clock::now();
    time_t tt = CHRONO::system_clock::to_time_t(now);
    tm t = *localtime(&tt);
    if (t.tm_hour > tme.hr || (t.tm_hour == tme.hr && t.tm_min > tme.min) || (t.tm_hour == tme.hr && t.tm_min == tme.min && t.tm_sec > tme.sec)) {
        tt += 86400;
        t = *localtime(&tt);
    }
    t.tm_sec = tme.sec;
    t.tm_min = tme.min;
    t.tm_hour = tme.hr;
    CHRONO::duration<time_t> diff = CHRONO::duration_cast<CHRONO::seconds>(CHRONO::system_clock::from_time_t(mktime(&t)) - now);
    return diff.count() + 1;
}

CHRONO::time_point<CHRONO::system_clock> d_nextTime(const DayTime tme) {
    time_t tt = CHRONO::system_clock::to_time_t(CHRONO::system_clock::now());
    tm t = *localtime(&tt);
    if (t.tm_hour > tme.hr || (t.tm_hour == tme.hr && t.tm_min > tme.min) || (t.tm_hour == tme.hr && t.tm_min == tme.min && t.tm_sec > tme.sec)) {
        tt += 86400;
        t = *localtime(&tt);
    }
    t.tm_sec = tme.sec;
    t.tm_min = tme.min;
    t.tm_hour = tme.hr;
    return CHRONO::system_clock::from_time_t(mktime(&t));
}

template<typename returntype, typename... args>
void routineThread(std::atomic_bool const& control, const DayTime tme, const time_t uintv, returntype(*func)(args...), args... arg) {
    std::atomic_bool& con = const_cast<std::atomic_bool&>(control);
    while (con) {
        while (uintv < d_untilNext(tme)) {
            if (!con) {
                break;
            }
            std::this_thread::sleep_for(CHRONO::seconds(uintv));
        }
        if (con) {
            std::this_thread::sleep_until(d_nextTime(tme));
            func(arg...);
            std::this_thread::sleep_for(CHRONO::seconds(1));
        }
    }
}

CE_STRING main_out = "/data/logs/rpi_out.txt";
CE_STRING apt_out = "/data/logs/apt_log.txt";
CE_STRING winb_out = "/data/logs/winsync.txt";

constexpr std::ios_base::openmode modes = (std::ios::app);

constexpr time_t update_intv = 10;

std::atomic_bool run = { true };
std::ofstream logger;

static void callback(int gpio, int level, uint32_t tick) {
    run = false;
    logger.open(main_out, modes);
    logger << dateStamp() << " : Button pressed - initiating shutdown" << newline;
    logger.close();
}

int main(int argc, char* argv[]) {
    logger.open(main_out, modes);
    logger << dateStamp() << " : System startup - PiHost version " << version << newline;
    logger.close();

    gpioInitialise();
    gpioHardwarePWM(p_fan, 25000, 400000);
    gpioSetMode(p_switch, PI_INPUT);
    gpioSetISRFunc(p_switch, RISING_EDGE, 0, callback);

    std::thread apt(routineThread<void, std::ofstream const&>, std::ref(run), DayTime(0, 0, 0), update_intv, aptRun, std::ofstream(apt_out, std::ios::trunc));
    std::thread winb(routineThread<void, std::ofstream const&>, std::ref(run), DayTime(16, 0, 0), update_intv, winBackup, std::ofstream(winb_out, std::ios::trunc));

    double temp;
    while (run) {
        temp = cpuTemp();
        if (temp >= 40) {
            logger.open(main_out, modes);
            logger << dateStamp() << " : High package temp of " << temp << " {CPU:" << getPercent() << "%}" << newline;
            logger.close();
        }
        std::this_thread::sleep_for(CHRONO::seconds(update_intv));
    }

    apt.join();
    winb.join();
    gpioTerminate();

    logger.open(main_out, modes);
    logger << dateStamp() << " : All threads closed - shutting down now" << newline << newline;
    logger.close();
    
    system("sudo halt");
    return 0;
}

/*RUNTIME LIST
* 
* handle args -> set output file?
* startup message
* init gpio, set fan, set shutdown
* create task threads -> bound to "run" bool
* start temp monitering loop
* ~wait for callback -> sighandler?
* end gpio, log shutdown
* call shutdown
*/