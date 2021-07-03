#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <unistd.h>
#include <fstream>

namespace cpu {
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

    float getTemp() {
        float systemp;
        FILE* thermal;

        thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
        fscanf(thermal, "%f", &systemp);
        fclose(thermal);
        systemp /= 1000;

        return systemp;
    }
}

double elapsed_time(std::chrono::time_point<std::chrono::system_clock>& start) {
    std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
    return diff.count();
}

static volatile bool run = true;

static void endCondition(void) {
    std::cin.get();
    run = false;
}

int main() {
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    std::thread end(endCondition);
    while (run) {
        std::cout << "CPU: {" << cpu::getTemp() << "*C | " << cpu::getPercent() << "%}\n";
    }
    end.join();
    std::cout << "Runtime: " << elapsed_time(start) << " seconds" << std::endl;
    return 0;
}