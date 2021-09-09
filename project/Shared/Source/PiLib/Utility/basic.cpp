#include "basic.h"

namespace pilib {
    uint16_t toNum(char num) {
        if (isdigit(num)) {
            return num - '0';
        }
        return 0;
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
    void replace(std::string& str, const std::string find, const char* replace) {
        str.replace(str.find(find), find.length(), replace);
    }
    std::streampos lineNum(uint line, std::istream& stream) {
        std::string dummy;
        for (uint i = 0; i < line; i++) {
            std::getline(stream, dummy);
        }
        return stream.tellg();
    }
    std::streampos lineNum(uint line, std::istream& stream, std::string& buf) {
        for (uint i = 0; i < line; i++) {
            std::getline(stream, buf);
        }
        return stream.tellg();
    }

    bool exists(const char* path) {
        struct stat buffer;
        return (stat(path, &buffer) == 0);
    }
    bool exists(const std::string& path) {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }

    const char* dateStamp() {
        time_t now = CHRONO::system_clock::to_time_t(CHRONO::system_clock::now());
        char* t = ctime(&now);
        t[strlen(t) - 1] = '\0';
        return t;
    }

    const char* dateStamp(time_t* tme) {
        char* t = ctime(tme);
        t[strlen(t) - 1] = '\0';
        return t;
    }

    std::string withTime(const char* message) {
        std::string ret(pilib::dateStamp());
        ret.reserve(strlen(message) + 4);
        ret.append(" : ");
        ret.append(message);
        return ret;
    }

    void exitError(const char* message) {
        perror(withTime(message).c_str());
        exit(EXIT_FAILURE);
    }
}