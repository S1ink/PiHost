#include "headers/utility.h"

namespace pilib {
#define OMODE std::_Ios_Openmode
    olstream::olstream() {
        this->fmode = 0;
        this->stream = &std::cout;
    }
    olstream::olstream(const char* file, const OMODE modes) {
        this->fmode = 1;
        this->fname = file;
        this->omodes = modes;
        this->stream = &std::cout;
    }
    olstream::olstream(std::ostream* stream) {
        this->fmode = 0;
        this->stream = stream;
    }
    olstream::olstream(const olstream& other) {
        this->fmode = other.fmode;
        this->fname = other.fname;
        this->omodes = other.omodes;
        if (other.stream != nullptr) {
            this->stream = other.stream;
        }
        else {
            this->stream = &std::cout;
        }
    }

    bool olstream::checkStream() {
        if (this->stream == nullptr) {
            this->stream = &std::cout;
            return 0;
        }
        return 1;
    }

    std::ostream& olstream::safeStream() {
        if (this->stream != nullptr) {
            return *(this->stream);
        }
        return std::cout;
    }

    void olstream::setStream(const char* file, const OMODE modes) {
       this->fmode = 1;
       this->fname = file;
       this->omodes = modes;
    }
    void olstream::setStream(std::ostream* stream) {
        this->fmode = 0;
        this->stream = stream;
    }

    void lstream::setFile(const char* file, OMODE modes) {
        this->file = file;
        this->modes = modes;
    }
    void lstream::setMode(OMODE modes) {
        this->modes = modes;
    }
    const char* lstream::getFile() { return file; }
    OMODE lstream::getModes() { return modes; }

    void lstream::openFile(const char* file, OMODE modes) {
        if (this->is_open()) {
            close();
        }
        this->file = file;
        this->modes = modes;
        open(file, modes);
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
        //std::cout << pilib::dateStamp() << " : Stream object copied. {" << this->is_open() << "}" << newline;
        file = other.file;
        modes = other.modes;

    }
    /*lstream::lstream(lstream&& other) {
        std::cout << pilib::dateStamp() << " : Stream object moved. {" << this->is_open() << "}" << newline;
        other.setData(file.c_str(), modes);
    }*/
    //lstream::~lstream() {std::cout << pilib::dateStamp() << " : Stream object deleted. {" << this->is_open() << "}" << newline;}

    const std::array<const char*, 31> SigHandle::sigmap = {
            "console hangup", "keyboard interrupt", "quit request",
            "illegal CPU instruction", "trace request", "abort",
            "invalid bus address", "floating point arithmetic error, likely div/0",
            "program killed (unblockable)", "custom signal, not from os",
            "segmentation violation - check for bad '*/&'", "custom signal - not an os error",
            "broken pipe", "alarm", "request to terminate", "stack fault",
            "child status has changed", "command to continue",
            "program stopped (unblockable)", "keyboard stop", "backround terminal read",
            "backround terminal write", "urgent socket condition", "CPU limit exceeded",
            "file size limit exceeded", "virtual alarm", "profiling alarm",
            "window size change", "IO now possible", "power restart (terminate)", "bad system call",
    };

    void SigHandle::sigIgnore(int signum) {
        std::cout << pilib::dateStamp() << " : Signal {" << signum << "} caught [" << sigmap[signum - 1] << "] -> ignored\n";
    }
    void SigHandle::sigTerminate(int signum) {
        std::cout << pilib::dateStamp() << " : Signal {" << signum << "} caught [" << sigmap[signum - 1] << "] -> calling exit\n";
        exit(0);
    }
    void SigHandle::ignoreBase(int signum, std::ostream& out) {
        out << pilib::dateStamp() << " : Signal {" << signum << "} caught [" << sigmap[signum - 1] << "] -> ignored\n";
    }
    void SigHandle::terminateBase(int signum, std::ostream& out) {
        out << pilib::dateStamp() << " : Signal {" << signum << "} caught [" << sigmap[signum - 1] << "] -> calling exit\n";
    }

    void SigHandle::reset(std::initializer_list<uint> ignore, std::initializer_list<uint> exit) {
        this->ign_sigs = ignore;
        this->exit_sigs = exit;
        if (this->ign != nullptr) {
            for (uint i = 0; i < this->ign_sigs.size(); i++) {
                signal(this->ign_sigs[i], this->ign);
            }
        }
        if (this->ex != nullptr) {
            for (uint i = 0; i < this->exit_sigs.size(); i++) {
                signal(this->exit_sigs[i], this->ex);
            }
        }
    }
    void SigHandle::setup(sigfunc ignore, sigfunc exit) {
        this->ign = ignore;
        this->ex = exit;
        for (uint i = 0; i < ign_sigs.size(); i++) {
            signal(ign_sigs[i], ignore);
        }
        for (uint i = 0; i < exit_sigs.size(); i++) {
            signal(exit_sigs[i], exit);
        }
    }
    void SigHandle::setup(sigfunc ignore, sigfunc exit, std::initializer_list<uint> ignorelist, std::initializer_list<uint> exitlist) {
        this->ign_sigs = ignorelist;
        this->exit_sigs = exitlist;
        this->ign = ignore;
        this->ex = exit;
        for (uint i = 0; i < ign_sigs.size(); i++) {
            signal(ign_sigs[i], ignore);
        }
        for (uint i = 0; i < exit_sigs.size(); i++) {
            signal(exit_sigs[i], exit);
        }
    }

    void Data::extractBool(const char* str) {
        std::istringstream text(str);
        text >> *(static_cast<bool*>(this->data));
    }
    void Data::extractChar(const char* str) {
        std::istringstream text(str);
        text >> *(static_cast<char*>(this->data));
    }
    void Data::extractInt(const char* str) {
        std::istringstream text(str);
        text >> *(static_cast<int*>(this->data));
    }
    void Data::extractUint(const char* str) {
        std::istringstream text(str);
        text >> *(static_cast<uint*>(this->data));
    }    
    void Data::extractFloat(const char* str) {
        std::istringstream text(str);
        text >> *(static_cast<float*>(this->data));
    }
    void Data::extractLong(const char* str) {
        std::istringstream text(str);
        text >> *(static_cast<long*>(this->data));
    }
    void Data::extractStr(const char* str) {
        *(static_cast<std::string*>(this->data)) = str;
    }

    void Data::extract(const char* str) {
        (this->*ext)(str);
    }

    ArgsHandler& ArgsHandler::get() {
        static ArgsHandler global;
        return global;
    }

    std::unordered_map<std::string, Data>* ArgsHandler::getVars() {
        return &(this->vars);
    }
    void ArgsHandler::insertVars(std::initializer_list< std::pair<const std::string, Data> > list) {
        this->vars.insert(list);
    }
    uint ArgsHandler::parse(int argc, char* argv[]) {
        uint noid = 0;
        std::string buffer;
        for (int i = 1; i < argc; i++) {
            std::istringstream arg(argv[i]);
            std::getline(arg, buffer, '=');
            auto result = this->vars.find(buffer);
            arg >> buffer;
            if (result != this->vars.end()) {
                result->second.extract(buffer.c_str());
            }
            else {
                noid += 1;
            }
        }
        return noid;
    }

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
            std::string line;
            while (fgets(buffer, 128, pipe)) {
                line.append(buffer);
                if (std::stringstream(line) >> result) {
                    break;
                }
                else {
                    line.clear();
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
            std::string line;
            bool found = false;
            while (fgets(buffer, 128, pipe)) {
                out << buffer;
                if (!found) {
                    line.append(buffer);
                    if (std::stringstream(line) >> result) {
                        found = true;
                    }
                    else {
                        line.clear();
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
            std::string line;
            while (fgets(buffer, 128, pipe)) {
                line.append(buffer);
                if (std::stringstream(line) >> result) {
                    break;
                }
                else {
                    line.clear();
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
            std::string line;
            bool found = false;
            while (fgets(buffer, 128, pipe)) {
                out << buffer;
                if (!found) {
                    line.append(buffer);
                    if (std::stringstream(line) >> result) {
                        found = true;
                    }
                    else {
                        line.clear();
                    }
                }
            }
        }
        pclose(pipe);
        return result;
    }

    //add output parser because rsync default is horrendous
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