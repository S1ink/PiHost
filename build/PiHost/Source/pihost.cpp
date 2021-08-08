#define INCLUDE_ALL
#define INFO
#define UTIL
#define TIMING
#define FILES
#define THREADING
#include "pilib.h"

CE_STR version = "1.3.4";

//all compatible thread modules must be in the from of ~ typedef void(*templatefunc)(const char*, std::ostream&)
namespace thmods {
    void aptRun(const char* message, std::ostream& output) {
        const char* sep = "*******************************************************************";
        pilib::StopWatch ptime(false);
        output << sep << "\nPihost internal APT updater initialized. (" << pilib::dateStamp() << ")\n" << sep;

        output << "\n\nUpdating Repos:\n" << sep << newline;
        int updates = pilib::aptUpdate(output);

        output << "\nUpgrading:\n" << sep << newline;
        int upgrades = pilib::aptUpgrade(output);

        output << "\nCleaning... ";
        system("sudo apt clean");
        output << "Finished cleaning.\n\n"
            << sep << "\nProcess finished at: " << pilib::dateStamp() 
            << "\nUpdates: " << updates << "\nUpgrades: " << upgrades << newline;
        ptime.pTotal(output);
    }

    void winBackup(const char* message, std::ostream& output) {
        const char* sep = "*****************************************************************";
        pilib::StopWatch ptime(false);
        output << sep << "\nPihost internal WinBackup initialized. (" << pilib::dateStamp() << ")\n" << sep << newline << newline;

        pilib::winSync(locations::external::winbackup, output);

        output << sep << "\nProcess finished at: " << pilib::dateStamp() << newline;
        ptime.pTotal(output);
    }

    void commandRun(const char* message, std::ostream& output) {
        pilib::StopWatch ptime;
        output << "Pihost internal command runner initialized. (" << pilib::dateStamp() << ")\n\n";

        pilib::exec(message, output);

        output << newline << "Process finished at: " << pilib::dateStamp() << newline;
        ptime.pTotal(output);
    }

    //add task that cleans up output files 
}

constexpr time_t update_intv = 10;
std::atomic_bool run = { true };
bool halt_on_exit = true;
pilib::lstream logger("/data/logs/rpi_out.txt", std::ios::app);

std::unordered_map<std::string, pilib::templatefunc> functions = {
        {"apt", thmods::aptRun},
        {"winbackup", thmods::winBackup},
        {"command", thmods::commandRun},
};


const std::array<std::string, 31> sigmap = {
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
const std::array<int, 17> int_sigs = {
    1, 5, 10, 12, 13, 14, 17, 18, 20, 21, 22, 23, 26, 27, 28, 29, 31
};
const std::array<int, 14> exit_sigs = {
    2, 3, 4, 6, 7, 8, 11, 15, 16, 24, 25, 30
};

void callback(int gpio, int level, uint32_t tick) {
    if (run != false) {
        logger.openOutput();
        logger << pilib::dateStamp() << " : Button pressed - initiating exit\n";
        logger.close();
        run = false;
    }
}

void sigIgnore(int signum) {
    std::string desc = sigmap[signum - 1];
    logger.openOutput();
    logger << pilib::dateStamp() << " : Signal {" << signum << "} caught [" << desc << "] -> ignored\n";
    logger.close();
}

void sigTerminate(int signum) {
    std::string desc = sigmap[signum - 1];
    logger.openOutput();
    logger << pilib::dateStamp() << " : Signal {" << signum << "} caught [" << desc << "] -> calling exit\n";
    logger.close();
    run = false;
    halt_on_exit = false;
}

//add argument support for paths and intervals
int main(int argc, char* argv[]) {
    std::vector<std::thread> threads;

    logger.openOutput();
    logger << pilib::dateStamp() << " : System startup - PiHost version " << version << newline;
    logger.close();

    gpioInitialise();
    gpioHardwarePWM(gpin::pi_fan, 25000, 400000);
    gpioSetMode(gpin::pi_power, PI_INPUT);
    gpioSetISRFunc(gpin::pi_power, RISING_EDGE, 0, callback);

    for (uint i = 0; i < int_sigs.size(); i++) {
        signal(int_sigs[i], sigIgnore);
    }
    for (uint i = 0; i < exit_sigs.size(); i++) {
        signal(exit_sigs[i], sigTerminate);
    }

    //add a way to update threads
    pilib::parseTasks(locations::external::tasks, std::cout, run, update_intv, threads, functions);

    logger.openOutput();
    logger << pilib::dateStamp() << " : " << threads.size() << " subprocesses started\n";
    logger.close();

    double temp;
    while (run) {
        temp = pilib::sys::cpuTemp();
        if (temp >= 40) {
            logger.openOutput();
            logger << pilib::dateStamp() << " : High package temp of " << temp << " - {CPU:" << pilib::sys::cpuPercent(CHRONO::seconds(1)) << "%}\n";
            logger.close();
        }
        std::this_thread::sleep_for(CHRONO::seconds(update_intv));
    }

    unsigned int i;
    for (i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
    gpioTerminate();

    logger.openOutput();
    logger << pilib::dateStamp() << " : " << i << " thread(s) closed - exitting now\n";    
    if (halt_on_exit) {
        logger << pilib::dateStamp() << " : Program set to halt on exit - calling shutdown\n\n";
        logger.close();
        system("sudo halt");    //"sudo shutdown -h now"
        return 0;
    }
    else {
        logger << newline; 
        logger.close();
        return 0;
    }
}

/*RUNTIME LIST
* 
* ~~~ handle args -> set vars ~~~
* startup message
* init gpio, set fan, set shutdown
* set sighandlers
* create task threads -> bound to "run" bool
* start temp monitering loop
* ~wait for callback -> sighandler?
* end gpio, log shutdown
* call shutdown
*/