#include "pilib.h"

namespace sysinfo {
    float cputemp() {
        float systemp;
        FILE* thermal;
        int n;

        thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
        n = fscanf(thermal, "%f", &systemp);
        fclose(thermal);
        systemp /= 1000;

        return systemp;
    }

    float gputemp() {
        char var[12];
        FILE* pipe = popen("vcgencmd measure_temp", "r");

        fscanf(pipe, "%s", var);
        float temp = (((float)var[5] - 48) * 10 + ((float)var[6] - 48) + ((float)var[8] - 48) / 10);
        pclose(pipe);

        return temp;
    }

    time_t now() {
        return time(NULL);
    }

    std::chrono::time_point<std::chrono::system_clock> _now() {
        return std::chrono::system_clock::now();
    }

    tm* struct_now() {
        time_t n = time(0);
        return localtime(&n);
    }

    tm* _struct_now() {
        time_t n = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        return localtime(&n);
    }

    char* dateStamp() {
        time_t now = time(0);
        return util::returnCut(ctime(&now));
    }

    double perf_timer(time_t start) {
        return difftime(time(0), start);
    }

    double elapsed_time(std::chrono::time_point<std::chrono::system_clock> start) {
        std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
        return diff.count();
    }
}

namespace util {
    std::string exec(std::string command) {
        char buffer[128];
        std::string result = "";
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            return "popen function failed";
        }
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL) {
                result += buffer;
            }
        }
        pclose(pipe);
        return result;
    }

    char* returnCut(char* text) {
        char* t = text;
        for (int i = 1; i < 3; i++) {
            if (t[strlen(t) - i] == '\n' || '\0') {
                t[strlen(t) - i] = ' ';
            }
        }
        return t;
    }

    void appLog(std::string file, std::string text) {
        std::fstream log;
        log.open(file, std::ios::out | std::ios::app);
        log << text;
        log.close();
    }
}

namespace gpio {
    int getStatus() {
        return gpioRead(p_status);
    }

    int getSwitch() {
        return gpioRead(p_switch);
    }

    void activateSwitch(int gpiodevice) {
        gpioWrite(gpiodevice, 1);
        gpioDelay(250000);
        gpioWrite(gpiodevice, 0);
    }

    void init() {
        gpioInitialise();
        gpioSetMode(p_switch, PI_INPUT);
        gpioSetMode(p_power, PI_OUTPUT);
        gpioSetMode(p_reset, PI_OUTPUT);
        gpioSetMode(p_status, PI_INPUT);
        gpioSetPullUpDown(p_status, PI_PUD_UP);
        gpioSetPullUpDown(p_switch, PI_PUD_UP);
        gpioHardwarePWM(18, 25000, 500000);
        return;
    }

    void gpio1() {
        int var;
        gpio::init();
        printf("Fan started\n");
        while (true) {
            printf("Enter a number: ");
            std::cin >> var;
            if (var == 1) {
                activateSwitch(p_power);
                printf("Activated power switch\n");
            }
            else if (var == 0) {
                std::cout << getStatus() << std::endl;
            }
            else if (var == -1) {
                break;
            }
        }
        gpioTerminate();
        return;
    }
}