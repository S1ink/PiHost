#include "pilib.h"

CE_STR version = "1.2.1";

//all compatible thread modules must be in the from of ~ typedef void(*templatefunc)(const char*, std::ostream&)
namespace thmods {
    void aptRun(const char* message, std::ostream& output) {
        timing::StopWatch ptime;
        output << "Pihost internal APT updater initialized. (" << info::dateStamp() << ")" << newline << newline;
        int updates = util::aptUpdate(output);
        output << newline << newline << " * * * * * * * * " << newline << newline;
        int upgrades = util::aptUpgrade(output);
        output << "Process finished at: " << info::dateStamp() << newline << "Total elapsed time: " << ptime.getDuration() << " seconds." << newline;
        output << "Updates: " << updates << newline << "Upgrades: " << upgrades;
    }

    void winBackup(const char* message, std::ostream& output) {
        timing::StopWatch ptime;
        output << "Pihost internal WinBackup initialized. (" << info::dateStamp() << ")" << newline << newline;
        files::csv::winSync(locations::external::winbackup, output);
        output << "Process finished at: " << info::dateStamp() << newline << "Total elapsed time: " << ptime.getDuration() << " seconds.";
    }

    void commandRun(const char* message, std::ostream& output) {
        timing::StopWatch ptime;
        output << "Pihost internal command runner initialized. (" << info::dateStamp() << ")" << newline << newline;
        util::exec(message, output);
        output << newline << "Process finished at: " << info::dateStamp() << newline << "Total elapsed time: " << ptime.getDuration() << " seconds.";
    }
}

CE_STR main_out = "/data/logs/rpi_out.txt";

constexpr std::ios_base::openmode modes = (std::ios::app);
constexpr time_t update_intv = 10;

std::map<std::string, threading::templatefunc> functions;
std::vector<std::thread> threads;

std::atomic_bool run = { true };
std::ofstream logger;

static void callback(int gpio, int level, uint32_t tick) {
    run = false;
    logger.open(main_out, modes);
    logger << info::dateStamp() << " : Button pressed - initiating shutdown" << newline;
    logger.close();
}

int main(int argc, char* argv[]) {
    logger.open(main_out, modes);
    logger << info::dateStamp() << " : System startup - PiHost version " << version << newline;
    logger.close();

    gpioInitialise();
    gpioHardwarePWM(gpin::pi_fan, 25000, 400000);
    gpioSetMode(gpin::pi_power, PI_INPUT);
    gpioSetISRFunc(gpin::pi_power, RISING_EDGE, 0, callback);

    functions.insert(std::pair<std::string, threading::templatefunc>("apt", thmods::aptRun));
    functions.insert(std::pair<std::string, threading::templatefunc>("winbackup", thmods::winBackup));
    functions.insert(std::pair<std::string, threading::templatefunc>("command", thmods::commandRun));

    threading::parseTasks(locations::external::tasks, std::cout, run, update_intv, threads, functions);
    logger.open(main_out, modes);
    logger << info::dateStamp() << " : " << threads.size() << " subprocesses started." << newline;
    logger.close();

    double temp;
    while (run) {
        temp = info::cputemp();
        if (temp >= 40) {
            logger.open(main_out, modes);
            logger << info::dateStamp() << " : High package temp of " << temp << " {CPU:" << info::cpu::getPercent() << "%}" << newline;
            logger.close();
        }
        std::this_thread::sleep_for(CHRONO::seconds(update_intv));
    }

    unsigned int i;
    for (i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
    gpioTerminate();

    logger.open(main_out, modes);
    logger << info::dateStamp() << " : " << i << " thread(s) closed - shutting down now" << newline << newline;
    logger.close();
    
    system("sudo halt");
    return 0;
}

/*RUNTIME LIST
* 
* handle args -> set output file?
* startup message
* init gpio, set fan, set shutdown
* create task threads -> bound to "run" bool
* start temp monitering loop
* ~wait for callback -> sighandler?
* end gpio, log shutdown
* call shutdown
*/