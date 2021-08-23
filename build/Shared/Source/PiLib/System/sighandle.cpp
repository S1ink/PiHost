#include "sighandle.h"

namespace pilib {
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
}