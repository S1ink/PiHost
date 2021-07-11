#include "pilib.h"

namespace info {
    float cputemp() {
        float systemp;
        FILE* thermal = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
        
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

            for (unsigned int i = 0; i < snapshot1.size(); i++) {
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

    char* dateStamp() {
        time_t now = CHRONO::system_clock::to_time_t(CHRONO::system_clock::now());
        char* t = ctime(&now);
        t[strlen(t)-1] = '\0';
        return t;
    }

    char* dateStamp(time_t* tme) {
        char* t = ctime(tme);
        t[strlen(t) - 1] = '\0';
        return t;
    }
}

namespace util {
    void exec(const char* command, std::ostream& output) {
        char buffer[128];
        FILE* pipe = popen(command, "r");
        if (!pipe) {
            output << "popen function failed";
            pclose(pipe);
        }
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL) {
                output << buffer;
            }
        }
        pclose(pipe);
    }

    int aptUpdate() {
        int result = 0;
        char buffer[128];
        std::string line = "";
        FILE* pipe = popen("sudo apt update", "r");
        if (!pipe) {
            result = -1;
        }
        else {
            while (fgets(buffer, 128, pipe)) {
                line.append(buffer);
                if (std::stringstream(line) >> result) {
                    break;
                }
                else {
                    line = "";
                }
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

    int aptUpgrade() {
        int result = 0;
        char buffer[128];
        std::string line = "";
        FILE* pipe = popen("sudo apt-get upgrade -y", "r");
        if (!pipe) {
            result = -1;
        }
        else {
            while (fgets(buffer, 128, pipe)) {
                line.append(buffer);
                if (std::stringstream(line) >> result) {
                    break;
                }
                else {
                    line = "";
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

    std::string rsync(const char* source, const char* destination, const char* options) {
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

    std::string rclone(const char* source, const char* destination, const char* mode) {
        std::stringstream command;
        char buffer[128];
        std::string output = "";
        command << "rclone " << mode << space << source << space << destination << " -vv";
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

    void s_rsync(const char* source, const char* destination, const char* options) {
        std::stringstream command;
        command << "sudo rsync " << options << space << source << space << destination;
        system(command.str().c_str());
    }

    void s_rclone(const char* source, const char* destination, const char* mode) {
        std::stringstream command;
        command << "rclone "  << mode << space << source << space << destination;
        system(command.str().c_str());
    }

    unsigned char clearEnd(std::string& str) {
        unsigned char ret = null;
        unsigned int len = str.length();
        for (unsigned char i = 1; i < 3; i++) {
            if (str[len-i] == 13 || str[len-i] == 10) {
                ret = str[len-i];
                str.pop_back();
            }
        }
        return ret;
    }

    void quickLog(std::string file, std::string text) {
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

namespace timing {
    time_t now() {
        return CHRONO::system_clock::to_time_t(CHRONO::system_clock::now());
    }

    tm* struct_now() {
        time_t n = CHRONO::system_clock::to_time_t(CHRONO::system_clock::now());
        return localtime(&n);
    }

    double perfTimer(CHRONO::time_point<CHRONO::high_resolution_clock>& start) {
        CHRONO::duration<double> diff = CHRONO::high_resolution_clock::now() - start;
        return diff.count();
    }

    StopWatch::StopWatch(bool now) {
        if (now) {
            start = CHRONO::high_resolution_clock::now();
        }
    }

    void StopWatch::setStart() {
        start = CHRONO::high_resolution_clock::now();
    }

    double StopWatch::getDuration() {
        CHRONO::duration<double> diff = CHRONO::high_resolution_clock::now() - start;
        return diff.count();
    }

    void StopWatch::pTotal() {
        CHRONO::duration<double> diff = CHRONO::high_resolution_clock::now() - start;
        std::cout << "Total elapsed time: " << diff.count() << " seconds" << newline;
    }

    void StopWatch::pTotal(std::ostream& output) {
        CHRONO::duration<double> diff = CHRONO::high_resolution_clock::now() - start;
        output << "Total elapsed time: " << diff.count() << " seconds" << newline;
    }

    time_d createTOD(uint16_t hr, uint16_t min, uint16_t sec) {
        return (hr * 3600) + (min * 60) + (sec);
    }

    time_d DayTime::toTOD() {
        return (hr * 3600) + (min * 60) + (sec);
    }

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

    time_t d_untilNext(DayTime& tme) {
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

    CHRONO::time_point<CHRONO::system_clock> d_nextTime(DayTime& tme) {
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
}

namespace files {
    namespace csv {
        Csv csvRead(const char* filepath) {
            files::csv::Csv data;
            std::ifstream reader(filepath);
            std::string linebuffer, segmentbuffer;

            while (std::getline(reader, linebuffer)) {
                data.emplace_back(Line());
                Line* line = &(data.back());
                std::istringstream linestream(linebuffer);

                while (std::getline(linestream, segmentbuffer, ',')) {
                    line->emplace_back(std::string());
                    std::string& segment = line->back();
                    segment = segmentbuffer;
                }
            }
            reader.close();
            return data;
        }

        void csvRead(const char* filepath, Csv& container) {
            std::ifstream reader(filepath);
            std::string linebuffer, segmentbuffer;

            while (std::getline(reader, linebuffer)) {
                container.emplace_back(Line());
                Line* line = &(container.back());
                std::istringstream linestream(linebuffer);

                while (std::getline(linestream, segmentbuffer, csvd)) {
                    line->emplace_back(std::string());
                    std::string& segment = line->back();
                    segment = segmentbuffer;
                }
            }
            reader.close();
            return;
        }

        bool winCheck(const char* filepath) {
            std::ifstream reader(filepath);
            std::string line;
            std::getline(reader, line);
            return (line == "name,source,destination,options");
        }

        void winSync(const char* filepath, std::ostream& output) {
            std::ifstream reader(filepath);
            std::string linebuffer;
            WinSync databuffer;

            std::getline(reader, linebuffer);
            char dlm = util::clearEnd(linebuffer);
            if (linebuffer != "name,source,destination,options") {
                output << "Instruction file is not in the correct format.\n";
            }
            else {
                while (std::getline(reader, linebuffer, dlm)) {
                    std::istringstream linestream(linebuffer);
                    std::getline(linestream, databuffer.name, csvd);
                    std::getline(linestream, databuffer.source, csvd);
                    std::getline(linestream, databuffer.destination, csvd);
                    if (std::getline(linestream, databuffer.options)) {
                        output << info::dateStamp() << " - Started syncing {" << databuffer.name << "}...\n* * * * * * * * * * *\n" << util::rsync(databuffer.source.c_str(), databuffer.destination.c_str(), databuffer.options.c_str()) << "* * * * * * * * * *\n\n\n";
                    }
                    else {
                        output << info::dateStamp() << " - Started syncing {" << databuffer.name << "}...\n* * * * * * * * * * *\n" << util::rsync(databuffer.source.c_str(), databuffer.destination.c_str()) << "* * * * * * * * * *\n\n\n";
                    }
                }
            }
            reader.close();
            return;
        }
    }
}

namespace threading {
    void streamWrapper(const char* message, std::ostream const& output, templatefunc func) {
        std::ostream& out = const_cast<std::ostream&>(output);
        if (typeid(output) == typeid(std::ofstream)) {
            std::ofstream fout;
            fout.basic_ios<char>::rdbuf(out.rdbuf());
            func(message, fout);
            fout.close();
        }
        else {
            func(message, out);
        }
    }

    void parseTasks(const char* filepath, std::ostream& output, std::atomic_bool& control, time_t th_uintv, std::vector<std::thread>& threads, std::map<std::string, templatefunc>& funcmap) {
        std::ifstream reader(filepath);
        std::string linebuffer, numbuff;
        files::TaskFile databuffer;
        templatefunc funcbuff;

        std::getline(reader, linebuffer);
        char dlm = util::clearEnd(linebuffer);
        if (linebuffer != "name,command,output,mode,hr,min,sec") {
            output << "Task file is not in the correct format.\n";
        }
        else {
            while (std::getline(reader, linebuffer)) {
                std::istringstream linestream(linebuffer);
                std::getline(linestream, databuffer.name, csvd);
                std::getline(linestream, databuffer.command, csvd);
                std::getline(linestream, databuffer.output, csvd);
                std::getline(linestream, databuffer.mode, csvd);
                std::getline(linestream, numbuff, csvd);
                {std::istringstream numstream(numbuff); numstream >> databuffer.tme.hr;}
                std::getline(linestream, numbuff, csvd);
                {std::istringstream numstream(numbuff); numstream >> databuffer.tme.min;}
                std::getline(linestream, numbuff, dlm);
                {std::istringstream numstream(numbuff); numstream >> databuffer.tme.sec;}

                auto search = funcmap.find(databuffer.name);
                funcbuff = search->second;

                std::ios_base::openmode mode;
                if (databuffer.mode == "a") {
                    mode = std::ios::app;
                }
                else if (databuffer.mode == "o") {
                    mode = std::ios::trunc;
                }

                std::thread th(threading::routineThread<void, const char*, std::ostream const&, threading::templatefunc>, std::ref(control), std::move(databuffer.tme), 10, threading::streamWrapper, databuffer.command.c_str(), std::ofstream(databuffer.output, mode), funcbuff);
                threads.emplace_back(std::move(th));
            }
        }
    }
}

namespace gpio {
    int getStatus() {
        return gpioRead(gpin::pc_status);
    }

    int getSwitch() {
        return gpioRead(gpin::pi_power);
    }

    void activateSwitch(int gpiodevice) {
        gpioWrite(gpiodevice, 1);
        gpioDelay(250000);
        gpioWrite(gpiodevice, 0);
    }

    void setNoctua(float percent) {
        int speed = 10000 * percent;
        gpioHardwarePWM(gpin::pi_fan, 25000, speed);
    }

    void init(float fanspeed) {
        gpioInitialise();
        gpioSetMode(gpin::pc_power, PI_OUTPUT);
        gpioSetMode(gpin::pc_reset, PI_OUTPUT);
        gpioSetMode(gpin::pc_status, PI_INPUT);
        gpioSetMode(gpin::pi_power, PI_INPUT);
        gpioHardwarePWM(gpin::pi_fan, 25000, ((int)(fanspeed * 10000)));
        return;
    }
}