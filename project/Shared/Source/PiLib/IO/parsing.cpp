#include "parsing.h"

namespace pilib {
    Csv csvRead(const char* filepath) {
        Csv data;
        std::ifstream reader(filepath);
        std::string linebuffer, segmentbuffer;
        while (std::getline(reader, linebuffer)) {
            data.emplace_back(Line());
            Line* line = &(data.back());
            std::istringstream linestream(linebuffer);
            while (std::getline(linestream, segmentbuffer, comma)) {
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
            while (std::getline(linestream, segmentbuffer, comma)) {
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
                std::getline(linestream, databuffer.name, comma);
                std::getline(linestream, databuffer.source, comma);
                std::getline(linestream, databuffer.destination, comma);
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



    /*void streamWrapper(const char* message, std::ostream const& output, TaskFunc func) {
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

    void streamWrapper(const char* message, lstream output, TaskFunc func) {
        output.openOutput();
        func(message, output);
        output.close();
    }*/

   /* void parseTasks(
        const char* filepath,
        std::ostream& output,
        std::atomic_bool& control,
        time_t th_uintv,
        std::vector<std::thread>& threads,
        std::unordered_map<std::string, TaskFunc>& funcmap
    ) {
        std::ifstream reader(filepath);
        std::string linebuffer, numbuff;
        TaskFile databuffer;
        TaskFunc funcbuff;
        std::getline(reader, linebuffer);
        char dlm = clearEnd(linebuffer);
        if (linebuffer != "name,command,output,mode,hr,min,sec") {
            output << "Task file is not in the correct format.\n";
        }
        else {
            while (std::getline(reader, linebuffer)) {
                std::istringstream linestream(linebuffer);
                std::getline(linestream, databuffer.name, comma);
                std::getline(linestream, databuffer.command, comma);
                std::getline(linestream, databuffer.output, comma);
                std::getline(linestream, databuffer.mode, comma);
                std::getline(linestream, numbuff, comma);
                {std::istringstream numstream(numbuff); numstream >> databuffer.tme.hr; }
                std::getline(linestream, numbuff, comma);
                {std::istringstream numstream(numbuff); numstream >> databuffer.tme.min; }
                std::getline(linestream, numbuff, dlm);
                {std::istringstream numstream(numbuff); numstream >> databuffer.tme.sec; }
                auto search = funcmap.find(databuffer.name);
                funcbuff = search->second;
                std::ios_base::openmode mode;
                if (databuffer.mode == "a") {
                    mode = std::ios::app;
                }
                else if (databuffer.mode == "o") {
                    mode = std::ios::trunc;
                }
                else {
                    mode = std::ios_base::openmode::_S_ios_openmode_end;
                }
                lstream stream(databuffer.output.c_str(), mode);
                std::thread th(
                    routineThread<void, const char*, lstream,
                    TaskFunc>,
                    std::ref(control),
                    std::move(databuffer.tme),
                    th_uintv,
                    static_cast<void(*)(const char*, lstream, TaskFunc)>(streamWrapper),
                    databuffer.command.c_str(),
                    stream,
                    funcbuff
                );
                threads.emplace_back(std::move(th));
            }
        }
    }*/
}