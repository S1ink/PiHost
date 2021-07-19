#define PILIB_FULL
#define STD_FULL
#include "pilib.h"

CE_STR version = "1.3.2";

//all compatible thread modules must be in the from of ~ typedef void(*templatefunc)(const char*, std::ostream&)
namespace thmods {
    void aptRun(const char* message, std::ostream& output) {
        pilib::StopWatch ptime(false);
        output << "Pihost internal APT updater initialized. (" << pilib::dateStamp() << ")" << newline;
        int updates = pilib::aptUpdate(output);
        output << newline << newline << " * * * * * * * * " << newline << newline;
        int upgrades = pilib::aptUpgrade(output);
        output << "Updates: " << updates << newline << "Upgrades: " << upgrades << newline;
        output << "Process finished at: " << pilib::dateStamp() << newline;
        ptime.pTotal(output);
    }

    void winBackup(const char* message, std::ostream& output) {
        pilib::StopWatch ptime(false);
        output << "Pihost internal WinBackup initialized. (" << pilib::dateStamp() << ")" << newline << newline;
        pilib::winSync(locations::external::winbackup, output);
        output << "Process finished at: " << pilib::dateStamp() << newline;
        ptime.pTotal(output);
    }

    void commandRun(const char* message, std::ostream& output) {
        pilib::StopWatch ptime;
        output << "Pihost internal command runner initialized. (" << pilib::dateStamp() << ")" << newline << newline;
        pilib::exec(message, output);
        output << newline << "Process finished at: " << pilib::dateStamp() << newline;
        ptime.pTotal(output);
    }
}

constexpr time_t update_intv = 10;

std::atomic_bool run = { true };
pilib::lstream logger("/data/logs/rpi_out.txt", std::ios::app);

static void callback(int gpio, int level, uint32_t tick) {
    run = false;
    logger.openOutput();
    logger << pilib::dateStamp() << " : Button pressed - initiating shutdown" << newline;
    logger.close();
}

int main(int argc, char* argv[]) {
    std::map<std::string, pilib::templatefunc> functions;
    std::vector<std::thread> threads;

    logger.openOutput();
    logger << pilib::dateStamp() << " : System startup - PiHost version " << version << newline;
    logger.close();

    gpioInitialise();
    gpioHardwarePWM(gpin::pi_fan, 25000, 400000);
    gpioSetMode(gpin::pi_power, PI_INPUT);
    gpioSetISRFunc(gpin::pi_power, RISING_EDGE, 0, callback);

    functions.insert(std::pair<std::string, pilib::templatefunc>("apt", thmods::aptRun));
    functions.insert(std::pair<std::string, pilib::templatefunc>("winbackup", thmods::winBackup));
    functions.insert(std::pair<std::string, pilib::templatefunc>("command", thmods::commandRun));

    pilib::parseTasks(locations::external::tasks, std::cout, run, update_intv, threads, functions);

    logger.openOutput();
    logger << pilib::dateStamp() << " : " << threads.size() << " subprocesses started." << newline;
    logger.close();

    double temp;
    while (run) {
        temp = pilib::sys::cpuTemp();
        if (temp >= 40) {
            logger.openOutput();
            logger << pilib::dateStamp() << " : High package temp of " << temp << " {CPU:" << pilib::sys::cpuPercent(CHRONO::seconds(1)) << "%}" << newline;
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
    logger << pilib::dateStamp() << " : " << i << " thread(s) closed - shutting down now" << newline << newline;
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