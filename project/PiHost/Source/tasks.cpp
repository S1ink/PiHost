#include "tasks.h"

TaskManager::TaskManager(
    std::string&& path,
    std::atomic_bool& rbool,
    const std::unordered_map<std::string, Task_f>& funcs,
    const olstream& out,
    time_t clock_intv
) :
    tfile(std::move(path)), external(&rbool), output(out), funcmap(funcs), clock(clock_intv)
{
	this->funcmap.insert({ "command", runCommand });
}
TaskManager::TaskManager(
	std::string&& path,
	std::atomic_bool& rbool,
	const std::unordered_map<std::string, Task_f>& funcs,
	olstream&& out,
	time_t clock_intv
) : 
	tfile(std::move(path)), external(&rbool), output(std::move(out)), funcmap(funcs), clock(clock_intv)
{
	this->funcmap.insert({ "command", runCommand });
}
TaskManager::TaskManager(
    const std::string& path,
    std::atomic_bool& rbool,
    const std::unordered_map<std::string, Task_f>& funcs,
    const olstream& out,
    time_t clock_intv
) :
    tfile(path), external(&rbool), output(out), funcmap(funcs), clock(clock_intv)
{
    this->funcmap.insert({ "command", runCommand });
}
TaskManager::~TaskManager() {
    end();
}

void TaskManager::insertFunc(std::initializer_list< std::pair<const std::string, Task_f> > map) {
	this->funcmap.insert(map);
}

void TaskManager::launchWorker(TaskManager* that) {
#ifdef TESTING
    std::cout << "Worker Launched: " << std::this_thread::get_id() << newline;
#endif
    std::ifstream reader;
    std::string linebuffer, numbuff;
    Task_t databuffer;
    Task_f funcbuff;
    std::atomic_bool swap = { false }, *swap_ptr = that->external;

    while (*(that->external)) {

        {   //EXECUTED
            reader.open(that->tfile);
            std::getline(reader, linebuffer);
            char dlm = clearEnd(linebuffer);

            if (linebuffer != "name,output,write,hr,min,sec,args") {
                that->output << withTime("Task file is not in the correct format - no tasks started\n");
            }
            else {
                swap_ptr = &swap;
                for (size_t i = 0; i < that->threads.size(); i++) {
                    if (that->threads[i].joinable()) {
                        that->threads[i].join();
                    }
                }
                if (&(that->external)) {
                    swap_ptr = that->external;
                }
                else {
                    return;
                }

                while (std::getline(reader, linebuffer)) {
                    std::istringstream linestream(linebuffer);
                    std::getline(linestream, databuffer.name, comma);
                    std::getline(linestream, databuffer.output, comma);
                    std::getline(linestream, databuffer.w_mode, comma);

                    std::getline(linestream, numbuff, comma);
                    { std::istringstream numstream(numbuff); numstream >> databuffer.tme.hr; }
                    std::getline(linestream, numbuff, comma);
                    { std::istringstream numstream(numbuff); numstream >> databuffer.tme.min; }
                    std::getline(linestream, numbuff, comma);
                    { std::istringstream numstream(numbuff); numstream >> databuffer.tme.sec; }
                    std::getline(linestream, databuffer.args, dlm);

                    auto search = that->funcmap.find(databuffer.name);
                    if (search != that->funcmap.end()) {
                        funcbuff = search->second;
                    }
                    else {
                        funcbuff = errorName;
                    }

                    std::ios_base::openmode mode;
                    if (databuffer.w_mode[0] == 'a') {
                        mode = std::ios::app;
                    }
                    else if (databuffer.w_mode[0] == 'o') {
                        mode = std::ios::trunc;
                    }
                    else {
                        mode = std::ios_base::openmode::_S_ios_openmode_end;
                    }

                    that->threads.emplace_back(p_routineThread<void, const char*, const olstream&>,
                        &swap_ptr,
                        databuffer.tme,
                        that->clock,
                        funcbuff,
                        databuffer.args.c_str(),
                        std::move(olstream(databuffer.output.c_str(), mode))
                    );

#ifdef TESTING
                    std::cout << "CREATED THREAD: " << that->threads.back().get_id()
                        << "\n\texternal: " << *swap_ptr
                        << "\n\ttime: " << databuffer.tme.hr << ":" << databuffer.tme.min << ':' << databuffer.tme.sec
                        << "\n\tclock: " << that->clock
                        << "\n\ttask: " << databuffer.name
                        << "\n\targs: " << databuffer.args
                        << "\n\toutput: " << databuffer.output << " : " << databuffer.w_mode << "\n\n";
#endif

                }
                reader.close();
            }
        }   //EXECUTED

        std::this_thread::sleep_for(CHRONO::seconds(1));    //make sure time is different so executed code isn't run more than once

        while (that->clock < untilDayTime({ 0, 0, 0 })) {
            if (!*(that->external)) {
                break;
            }
            std::this_thread::sleep_for(CHRONO::seconds(that->clock));
        }
        if (*(that->external)) {
            std::this_thread::sleep_until(nextDayTime({ 0, 0, 0 }));
        }
    }
    for (size_t i = 0; i < that->threads.size(); i++) {
        if (that->threads[i].joinable()) {
            that->threads[i].join();
        }
    }
}
void TaskManager::launch() {
    if (!this->launched.joinable()) {
        this->launched = std::move(std::thread(launchWorker, this));
    }
}
void TaskManager::end() {
    if (this->launched.joinable()) {
        this->launched.join();
    }
}

size_t TaskManager::getThreads() {
    return this->threads.size();
}

void TaskManager::runCommand(const char* message, const olstream& logs) {
    olstream out(logs);
    StopWatch ptime("Total elapsed time", logs, 0);
    ((out <<= "Pihost internal command runner initialized. (") <= dateStamp()) < ")\n\n";

    pilib::exec(message, out.open());

    out <= newline <= "Process finished at: " <= dateStamp() < newline;
    ptime.end();
}
void TaskManager::errorName(const char* message, const olstream& logs) {
    olstream out(logs);
    out << withTime("Task name not found\n");
}