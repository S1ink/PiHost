#include "headers/threading.h"

namespace pilib {
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

    void streamWrapper(const char* message, lstream output, templatefunc func) {
        output.openOutput();
        func(message, output);
        output.close();
    }

    void parseTasks(
        const char* filepath, 
        std::ostream& output, 
        std::atomic_bool& control, 
        time_t th_uintv, 
        std::vector<std::thread>& threads, 
        std::map<std::string, 
        templatefunc>& funcmap
    ) {
        std::ifstream reader(filepath);
        std::string linebuffer, numbuff;
        TaskFile databuffer;
        templatefunc funcbuff;
        std::getline(reader, linebuffer);
        char dlm = clearEnd(linebuffer);
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
                {std::istringstream numstream(numbuff); numstream >> databuffer.tme.hr; }
                std::getline(linestream, numbuff, csvd);
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
                    templatefunc>, 
                    std::ref(control), 
                    std::move(databuffer.tme), 
                    th_uintv, 
                    static_cast<void(*)(const char*, lstream, templatefunc)>(streamWrapper), 
                    databuffer.command.c_str(), 
                    stream, 
                    funcbuff
                );
                threads.emplace_back(std::move(th));
            }
        }
    }
}