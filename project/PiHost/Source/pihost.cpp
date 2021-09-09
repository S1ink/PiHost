#define INCLUDE_ALL
#define INFO
#define UTIL
#define TIMING
#define FILES
#define THREADING
#include "pilib.h"

CE_STR version = "1.3.6";

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

std::atomic_bool run = { true };
bool halt_on_exit = true;
pilib::lstream logger("/data/logs/rpi_out.txt", std::ios::app);

std::unordered_map<std::string, pilib::templatefunc> functions = {
        {"apt", thmods::aptRun},
        {"winbackup", thmods::winBackup},
        {"command", thmods::commandRun},
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
    logger.openOutput();
    pilib::SigHandle::ignoreBase(signum, logger);
    logger.close();
}

void sigTerminate(int signum) {
    logger.openOutput();
    pilib::SigHandle::terminateBase(signum, logger);
    logger.close();
    run = false;
    halt_on_exit = false;
}

//add argument support for paths and intervals
int main(int argc, char* argv[]) {
    std::vector<std::thread> threads;

    time_t update_intv = 10;
    uint fan_pin = gpin::pi_fan;
    uint button_pin = gpin::pi_power;
    float fan_speed = 40.f;
    int warn_temp = 40;
    std::string taskfile_path = locations::external::tasks;

    if (argc > 1) {
        pilib::ArgsHandler& args = pilib::ArgsHandler::get();
        args.insertVars({
            {"fanpin", &fan_pin},
            {"buttonpin", &button_pin},
            {"fanspeed", &fan_speed},
            {"warningtemp", &warn_temp},
            {"pollinterval", &update_intv},
            {"tasks", &taskfile_path},
            {"halt", &halt_on_exit}
        });
        args.parse(argc, argv);
    }

    logger.openOutput();
    logger << pilib::dateStamp() << " : Startup - PiHost v." << version << newline;
    logger.close();

    gpioInitialise();
    gpioHardwarePWM(fan_pin, 25000, fan_speed*10000);
    gpioSetMode(button_pin, PI_INPUT);
    gpioSetISRFunc(button_pin, RISING_EDGE, 0, callback);

    pilib::SigHandle::get().setup(sigIgnore, sigTerminate);

    //add a way to update threads
    pilib::parseTasks(taskfile_path.c_str(), std::cout, run, update_intv, threads, functions);

    logger.openOutput();
    logger << pilib::dateStamp() << " : " << threads.size() << " threads launched\n";
    logger.close();

    double temp;
    while (run) {
        temp = pilib::sys::cpuTemp();
        if (temp >= warn_temp) {
            logger.openOutput();
            logger << pilib::dateStamp() << " : High SoC temp of " << temp << "*C - {CPU%:" << pilib::sys::cpuPercent(CHRONO::seconds(1)) << "}\n";
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

//VARIABLES
/*
PATHS:
 - task file
 - output

SETTINGS:
 - polling interval
 - context {headless/terminal}
 - warning temp
 - gpio pins {fan, button}

*/

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