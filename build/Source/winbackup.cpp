#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <string.h>
#include <ctime>

constexpr char* version = "1.0.3";

constexpr char newline = 10;
constexpr char space = 32;

struct WinSync {
    std::string name;
    std::string source;
    std::string destination;
    std::string options;
};

char* dateStamp() {
    time_t now = time(0);
    char* t = ctime(&now);
    t[strlen(t) - 1] = '\0';
    return t;
}

double elapsed_time(std::chrono::time_point<std::chrono::system_clock>& start) {
    std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
    return diff.count();
}

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

int main() {
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    std::cout << "WinBackup version " << version << " initialized. (" << dateStamp() << ")" << newline << newline;
    winSync("/data/pihost/rsyncbackup.csv", std::cout);
    std::cout << "Program finished at: " << dateStamp() << newline << "Total elapsed time: " << elapsed_time(start) << " seconds." << std::endl;
	return 0;
}