#include "headers/files.h"

namespace pilib {
    Csv csvRead(const char* filepath) {
        Csv data;
        std::ifstream reader(filepath);
        std::string linebuffer, segmentbuffer;
        while (std::getline(reader, linebuffer)) {
            data.emplace_back(Line());
            Line* line = &(data.back());
            std::istringstream linestream(linebuffer);
            while (std::getline(linestream, segmentbuffer, csvd)) {
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

    void winSync(const char* filepath, std::ostream& output) {
        std::ifstream reader(filepath);
        std::string linebuffer;
        std::getline(reader, linebuffer);
        char dlm = clearEnd(linebuffer);
        if (dlm == null) {
            dlm = newline;
        }
        if (linebuffer != "name,source,destination,options") {
            output << "Instruction file is not in the correct format.\n";
        }
        else {
            std::ostringstream logs;
            WinSync databuffer;
            const char* beg = ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
            const char* end = "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<";
            while (std::getline(reader, linebuffer, dlm)) {
                std::istringstream linestream(linebuffer);
                std::getline(linestream, databuffer.name, csvd);
                std::getline(linestream, databuffer.source, csvd);
                std::getline(linestream, databuffer.destination, csvd);
                if (std::getline(linestream, databuffer.options)) {
                    rsync(logs, databuffer.source.c_str(), databuffer.destination.c_str(), databuffer.options.c_str());
                    output << "Backing up [" << databuffer.name << "] - (" << dateStamp() << "):\n" 
                        << beg << newline << logs.str() << end << "\n\n\n\n";
                }
                else {
                    rsync(logs, databuffer.source.c_str(), databuffer.destination.c_str());
                    output << "Backing up [" << databuffer.name << "] - (" << dateStamp() << "):\n"
                        << beg << newline << logs.str() << end << "\n\n\n\n";
                }
            }
        }
        reader.close();
        return;
    }
}