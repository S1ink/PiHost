#include <iostream>
#include <fstream>
#include <ctime>
#include <unistd.h>
#include <string.h>

#include "pigpio.h"

constexpr char* version = "1.0.1";

constexpr int p_fan = 18;
constexpr int p_switch = 3;

constexpr char newline = '\n';

constexpr char* logs = "/data/logs/rpi_out.txt";
constexpr std::ios_base::openmode modes = (std::ios::out | std::ios::app);

static volatile bool run = true;
std::fstream logger;
double temp;

float cpuTemp() {
    float systemp;
    FILE* thermal;

    thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    fscanf(thermal, "%f", &systemp);
    fclose(thermal);
    systemp /= 1000;

    return systemp;
}

//float gpuTemp() {
//    char var[12];
//    FILE* pipe = popen("vcgencmd measure_temp", "r");
//
//    fscanf(pipe, "%s", var);
//    float temp = (((float)var[5] - 48) * 10 + ((float)var[6] - 48) + ((float)var[8] - 48) / 10);
//    pclose(pipe);
//
//    return temp;
//}

char* dateStamp() {
    time_t now = time(0);
    char* t = ctime(&now);
    t[strlen(t) - 1] = '\0';
    return t;
}

static void callback(int gpio, int level, uint32_t tick) {
    run = false;
    logger.open(logs, modes);
    logger << dateStamp() << " : Button pressed - shutting down" << newline;
    logger.close();
}

int main(int argc, char* argv[]) {
    logger.open(logs, modes);
    logger << dateStamp() << " : System startup - PiHost version " << version << newline;
    logger.close();
    gpioInitialise();
    gpioHardwarePWM(p_fan, 25000, 400000);
    gpioSetMode(p_switch, PI_INPUT);
    gpioSetISRFunc(p_switch, RISING_EDGE, 0, callback);
    while (run) {
        temp = cpuTemp();
        if (temp >= 40) {
            logger.open(logs, modes);
            logger << dateStamp() << " : High package temp of " << temp << newline;
            logger.close();
        }
        sleep(10);
    }
    gpioTerminate();
    logger.open(logs, modes);
    logger << dateStamp() << " : All processes ended - shutting down" << newline << newline;
    logger.close();
    system("sudo halt");
    return 0;
}