#include "pilib.h"

namespace info {
    float cputemp() {
        float systemp;
        FILE* thermal;
        
        thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
        fscanf(thermal, "%f", &systemp);
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

    namespace cpu {
        size_t getIdle(const LineParse& data) {
            return data.time[3] + data.time[4];
        }

        size_t getActive(const LineParse& data) {
            return data.time[0] + data.time[1] + data.time[2] + data.time[5] + data.time[6] + data.time[7] + data.time[8] + data.time[9];
        }

        void readMain(LineParse& container) {
            std::ifstream reader(locations::stats::cpu);

            reader >> container.title;
            for (int i = 0; i < parsable_states; i++) {
                reader >> container.time[i];
            }
            reader.close();
        }

        void readVector(std::vector<LineParse>& container) {
            std::ifstream reader(locations::stats::cpu);
            std::string linebuffer;

            while (std::getline(reader, linebuffer)) {
                if (!linebuffer.compare(0, 3, "cpu")) {
                    std::istringstream stream(linebuffer);

                    container.emplace_back(LineParse());
                    LineParse& line = container.back();

                    stream >> line.title;

                    for (int i = 0; i < parsable_states; i++) {
                        stream >> line.time[i];
                    }
                }
            }
            reader.close();
        }

        void convertData(LineParse& snapshot1, LineParse& snapshot2, ActivityData& output) {
            float active = static_cast<float>(getActive(snapshot2) - getActive(snapshot1));
            float idle = static_cast<float>(getIdle(snapshot2) - getIdle(snapshot1));
            float total = active + idle;
            output.title = snapshot1.title;
            output.activity = 100.f * (active / total);
        }

        void convertVectorData(std::vector<LineParse>& snapshot1, std::vector<LineParse>& snapshot2, std::vector<ActivityData>& output) {
            float active, idle, total;

            for (int i = 0; i < snapshot1.size(); i++) {
                output.emplace_back(ActivityData());
                ActivityData& entry = output.back();

                active = static_cast<float>(getActive(snapshot2[i]) - getActive(snapshot1[i]));
                idle = static_cast<float>(getIdle(snapshot2[i]) - getIdle(snapshot1[i]));
                total = active + idle;

                entry.title = snapshot1[i].title;
                entry.activity = 100.f * (active / total);
            }
        }

        void takeSample(std::vector<LineParse>& vector1, std::vector<LineParse>& vector2, std::vector<ActivityData>& result, unsigned int sample_interval) {
            vector1.clear();
            vector2.clear();
            result.clear();
            readVector(vector1);
            sleep(sample_interval);
            readVector(vector2);
            convertVectorData(vector1, vector2, result);
        }

        float getPercent(unsigned int wait) {
            LineParse time1, time2;
            readMain(time1);
            sleep(wait);
            readMain(time2);
            float active = static_cast<float>(getActive(time2) - getActive(time1));
            float total = active + (static_cast<float>(getIdle(time2) - getIdle(time1)));
            return 100.f * (active / total);
        }
    }

    Cpu::Cpu(unsigned int sample_time) : sample_rate(sample_time) {
        sample1.reserve(5);
        sample2.reserve(5);
        result.reserve(5);
    }

    std::vector<info::cpu::ActivityData>& Cpu::getSample() {
        sample1.clear();
        sample2.clear();
        result.clear();
        info::cpu::takeSample(sample1, sample2, result);
        return result;
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
        char* t = ctime(&now);
        t[strlen(t) - 1] = '\0';
        return t;
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

    char* cutNewline(char* text) {
        char* t = text;
        for (int i = 1; i < 3; i++) {
            if (t[strlen(t) - i] == '\n') {
                t[strlen(t) - i] = '\0';
            }
        }
        return t;
    }

    void singleLog(std::string file, std::string text) {
        std::fstream log;
        log.open(file, std::ios::out | std::ios::app);
        log << text;
        log.close();
    }

    void util::logger::log(std::string text) {
        scribe.open(path, modes);
        scribe << text;
        scribe.close();
    }

    void util::logger::stampedLog(std::string text) {
        scribe.open(path, modes);
        scribe << info::dateStamp() << text;
        scribe.close();
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

    void setNoctua(int percent) {
        int speed = 10000*percent;
        gpioHardwarePWM(p_fan, 25000, speed);
    }

    void init() {
        gpioInitialise();
        gpioSetMode(p_power, PI_OUTPUT);
        gpioSetMode(p_reset, PI_OUTPUT);
        gpioSetMode(p_switch, PI_INPUT);
        gpioSetMode(p_status, PI_INPUT);
        //gpioSetPullUpDown(p_status, PI_PUD_UP);
        //gpioSetPullUpDown(p_switch, PI_PUD_UP);
        gpioHardwarePWM(p_fan, 25000, 500000);
        return;
    }
}