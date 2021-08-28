#include "output.h"

namespace pilib {
#define OMODE std::_Ios_Openmode
    olstream::olstream() {
        //std::cout << "Constructed!\n";
        this->fmode = 0;
        this->stream = &std::cout;
    }
    olstream::olstream(const char* file, const OMODE modes) {
        //std::cout << "Constructed with f-path!\n";
        this->fmode = 1;
        this->fname = file;
        this->omodes = modes;
        this->stream = &std::cout;
    }
    olstream::olstream(const OMODE modes) {
        this->fmode = 0;
        this->omodes = modes;
        this->stream = &std::cout;
    }
    olstream::olstream(std::ostream* stream) {
        //std::cout << "Constructed with stream!\n";
        this->fmode = 0;
        this->stream = stream;
    }
    olstream::olstream(const olstream& other) {
        //std::cout << "Copied!\n";
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
    olstream::olstream(olstream&& other) {
        //std::cout << "Moved!\n";
        this->fmode = other.fmode;
        this->stream = other.stream;
        this->fname = other.fname;
        this->omodes = other.omodes;

        //other.fmode = 0;
        other.stream = nullptr;
        //other.fname = nullptr;
        //other.omodes = (OMODE)0;
    }

    olstream& olstream::operator=(olstream&& other) {
        if (this != &other) {
            //std::cout << "Assigned!\n";
            this->fmode = other.fmode;
            this->stream = other.stream;
            this->fname = other.fname;
            this->omodes = other.omodes;

            other.stream = nullptr;
        }
        return *this;
    }
    olstream& olstream::operator=(const olstream& other) {
        //std::cout << "Copy Assigned!\n";
        this->fmode = other.fmode;
        this->fname = other.fname;
        this->omodes = other.omodes;
        if (other.stream != nullptr) {
            this->stream = other.stream;
        }
        else {
            this->stream = &std::cout;
        }
        return *this;
    }

    bool olstream::safeStream() {
        if (this->stream == nullptr) {
            this->stream = &std::cout;
            return 0;
        }
        return 1;
    }
    std::ostream& olstream::streamGuard() {
        if (this->stream != nullptr) {
            return *(this->stream);
        }
        return std::cout;
    }

    void olstream::setModes(const OMODE modes) {
        this->omodes = modes;
    }
    void olstream::addModes(const OMODE modes) {
        this->omodes | modes;
    }
    void olstream::setStream(const char* file) {
        this->fmode = 1;
        this->fname = file;
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

    std::ostream& olstream::getStream() {
        return streamGuard();
    }

    /*void olstream::test() {
        std::cout << '\t' << fmode << newline;
        std::cout << '\t' << (bool)file << newline;;
        std::cout << '\t' << stream << newline;
        std::cout << '\t' << fname << newline;
        std::cout << '\t' << omodes << newline;
    }*/

    lstream::lstream(lstream&& other) {
        //somehow move parent class instance?
        this->file = other.file;
        this->modes = other.modes;
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
}