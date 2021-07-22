#include "headers/utility.h"

namespace pilib {
#define OMODE std::_Ios_Openmode
    void lstream::setData(const char* file, OMODE modes) {
        this->file = file;
        this->modes = modes;
    }
    void lstream::setData(OMODE modes) {
        this->modes = modes;
    }

    const char* lstream::getFile() { return file.c_str(); }
    OMODE lstream::getModes() { return modes; }
    std::string* lstream::rawFile() { return &file; }
    OMODE* lstream::rawModes() { return &modes; }

    void lstream::openFile(const char* file, OMODE modes) {
        if (!(this->is_open())) {
            this->file = file;
            this->modes = modes;
            open(file, modes);
        }
    }
#undef OMODE
    void lstream::openInput() {
        if (this->is_open()) {
            close();
        }
        open(file, ((modes | std::ios::in) & ~(std::ios::out | std::ios::trunc)));
    }
    void lstream::openOutput() {
        if (this->is_open()) {
            close();
        }
        open(file, ((modes | std::ios::out) & ~std::ios::in));
    }

    lstream::lstream(const lstream& other) {
        std::cout << pilib::dateStamp() << " : Stream object copied. {" << this->is_open() << "}" << newline;
        file = other.file;
        modes = other.modes;

    }
    /*lstream::lstream(lstream&& other) {
        std::cout << pilib::dateStamp() << " : Stream object moved. {" << this->is_open() << "}" << newline;
        other.setData(file.c_str(), modes);
    }*/
    //lstream::~lstream() {std::cout << pilib::dateStamp() << " : Stream object deleted. {" << this->is_open() << "}" << newline;}

    void exec(const char* command, std::ostream& output) {
        FILE* pipe = popen(command, "r");
        if (!pipe) {
            output << "popen function failed";
        }
        else {
            char buffer[128];
            while (fgets(buffer, 128, pipe) != NULL) {
                output << buffer;
            }
        }
        pclose(pipe);
    }

    int aptUpdate() {
        int result = 0;
        FILE* pipe = popen("sudo apt update", "r");
        if (!pipe) {
            result = -1;
        }
        else {
            char buffer[128];
            std::string line = "";
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
        FILE* pipe = popen("sudo apt update", "r");
        if (!pipe) {
            result = -1;
        }
        else {
            char buffer[128];
            std::string line = "";
            bool found = false;
            while (fgets(buffer, 128, pipe)) {
                out << buffer;
                if (!found) {
                    line.append(buffer);
                    if (std::stringstream(line) >> result) {
                        found = true;
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
        FILE* pipe = popen("sudo apt-get upgrade -y", "r");
        if (!pipe) {
            result = -1;
        }
        else {
            char buffer[128];
            std::string line = "";
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
        FILE* pipe = popen("sudo apt-get upgrade -y", "r");
        if (!pipe) {
            result = -1;
        }
        else {
            char buffer[128];
            std::string line = "";
            bool found = false;
            while (fgets(buffer, 128, pipe)) {
                out << buffer;
                if (!found) {
                    line.append(buffer);
                    if (std::stringstream(line) >> result) {
                        found = true;
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

    void rsync(std::ostream& output, const char* source, const char* destination, const char* options) {
        std::ostringstream command;
        command << "sudo rsync " << options << space << source << space << destination;
        FILE* pipe = popen(command.str().c_str(), "r");
        if (!pipe) {
            output << "popen function failed";
        }
        else {
            char buffer[128];
            while (fgets(buffer, 128, pipe) != NULL) {
                output << buffer;
            }
        }
        pclose(pipe);
    }

    void rclone(std::ostream& output, const char* source, const char* destination, const char* mode) {
        std::ostringstream command;
        command << "rclone " << mode << space << source << space << destination << " -vv";
        FILE* pipe = popen(command.str().c_str(), "r");
        if (!pipe) {
            output << "popen function failed";
        }
        else {
            char buffer[128];
            while (fgets(buffer, 128, pipe) != NULL) {
                output << buffer;
            }
        }
        pclose(pipe);
    }

    void rsync(const char* source, const char* destination, const char* options) {
        std::ostringstream command;
        command << "sudo rsync " << options << space << source << space << destination;
        system(command.str().c_str());
    }

    void rclone(const char* source, const char* destination, const char* mode) {
        std::ostringstream command;
        command << "rclone " << mode << space << source << space << destination;
        system(command.str().c_str());
    }

    char clearEnd(std::string& str) {
        char ret = null;
        uint len = str.length();
        for (unsigned char i = 1; i < 3; i++) {
            if (str[len - i] == 13 || str[len - i] == 10) {
                ret = str[len - i];
                str.pop_back();
            }
        }
        return ret;
    }

    bool exists(const char* path) {
        struct stat buffer;
        return (stat(path, &buffer) == 0);
    }

    void replace(std::string& str, const std::string find, const char* replace) {
        str.replace(str.find(find), find.length(), replace);
    }

    void exitError(const char* message) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}