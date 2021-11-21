#include <iostream>
#include <sstream>
#include <atomic>
#include <thread>
#include <pigpio.h>

#include "output.h"
#include "timing.h"
#include "basic.h"
#include "sighandle.h"
#include "program.h"
#include "resources.h"
#include "unix/syscom.h"
#include "unix/stats.h"
//#include <rpi/pigpio.h>

//#define TESTING

#include "tasks.h"
#include "parse.h"

CE_STR version = "1.3.7";

#define GPIO_FAN 18
#define GPIO_PWR 3

//all compatible thread modules must be in the from of ~ typedef void(*)(const char*, std::ostream&)
void aptRun(const std::string& message, const olstream& logs) {
    olstream outl(logs);
    std::ostringstream out;

    const char* sep = "*******************************************************************";
    StopWatch ptime("Total elapsed time", &out, 0);

    out << sep << "\nPihost internal APT updater initialized. (" << dateStamp() << ")\n" << sep;

    out << "\n\nUpdating Repos:\n" << sep << newline;
    int updates = aptUpdate(out);

    out << "\nUpgrading:\n" << sep << newline;
    int upgrades = aptUpgrade(out);

    out << "\nCleaning... ";
    system("sudo apt clean");
    out << "Finished cleaning.\n\n"
        << sep << "\nProcess finished at: " << dateStamp()
        << "\nUpdates: " << updates << "\nUpgrades: " << upgrades << newline;
    ptime.end();

    outl << out.str();
}

void winBackup(const std::string& message, const olstream& logs) {
    olstream outl(logs);
    std::ostringstream out;

    const char* sep = "*****************************************************************";
    StopWatch ptime("Total elapsed time", &out, 0);

    out << sep << "\nPihost internal WinBackup initialized. (" << dateStamp() << ")\n" << sep << "\n\n";

    winSync(message.c_str(), out);

    out << sep << "\nProcess finished at: " << dateStamp() << newline;
    ptime.end();

    outl << out.str();
}

std::atomic_bool run = { true };
bool halt_on_exit = true;
olstream m_out;

#ifndef TESTING
void callback(int gpio, int level, uint32_t tick) {
    if (run != false) {
        m_out << withTime("Button pressed - initiating exit\n");
        run = false;
    }
}
#endif

void sigShutdown(int signum) {
    SigHandle::printExit(signum);
    run = false;
    halt_on_exit = false;
}


// ************ MAIN ************

int main(int argc, char* argv[]) {

#ifdef TESTING
    std::cout << " ******* TESTING MODE ******* \n\n";
#endif
    progdir.setDir(argv[0]);

    time_t update_intv = 10;
    int warn_temp = 40;
#ifndef TESTING
    float fan_speed = 40.f;
    uint fan_pin = GPIO_FAN;
    uint button_pin = GPIO_PWR;
    std::string main_output(std::move(progdir.getDirSlash() + "pihost.txt"));
#endif
    std::string taskfile_path(std::move(progdir.getDirSlash() + "tasks.csv"));    

    if (argc > 1) {
        ArgsHandler& args = ArgsHandler::get();
        args.insertVars({
#ifndef TESTING
            {"fanpin", &fan_pin},
            {"buttonpin", &button_pin},
            {"fanspeed", &fan_speed},
            {"output", &main_output},
#endif
            {"warningtemp", &warn_temp},
            {"pollinterval", &update_intv},
            {"tasks", &taskfile_path},
            {"halt", &halt_on_exit}
        });
        args.parse(argc, argv);
    }

#ifndef TESTING 
    m_out.setStream(main_output.c_str(), std::ios::app);
#endif

    (((m_out <<= dateStamp()) <= " : Startup - PiHost v.") <= version) < newline;

#ifndef TESTING
    gpioInitialise();
    gpioHardwarePWM(fan_pin, 25000, fan_speed*10000);
    gpioSetMode(button_pin, PI_INPUT);
    gpioSetISRFunc(button_pin, RISING_EDGE, 0, callback);
#endif

    sig_handle.setLog(m_out);
    sig_handle.setup(sigShutdown);

    TaskManager tasks(
        std::move(taskfile_path),
        run,
        { {"apt", aptRun}, {"winbackup", winBackup}, },
        m_out,
        update_intv
    );

    /*std::cout << "safe?\n\n";
    std::this_thread::sleep_for(CHRONO::seconds(3));*/

    tasks.launch();

    std::this_thread::sleep_for(CHRONO::seconds(1));
    (((m_out <<= dateStamp()) <= " : ") <= tasks.getThreads()) < " task threads started\n";

    //TODO: PID loop
    float temp;
    while (run) {
        temp = sys::cpuTemp();
        if (temp >= warn_temp) {
            (((((m_out <<= dateStamp()) <= " : High SoC temp of ") <= temp) <= "*C - {CPU%:") <= sys::cpuPercent(CHRONO::seconds(1))) < "}\n";
        }
        std::this_thread::sleep_for(CHRONO::seconds(update_intv));
    }

    tasks.end();
#ifndef TESTING
    gpioTerminate();
#endif

    (m_out <<= dateStamp()) <= " : All thread(s) closed - exitting now\n";   
#ifndef TESTING
    if (halt_on_exit) {
        m_out <= dateStamp() < " : Program set to halt on exit - calling shutdown\n\n";
        system("sudo halt");    //"sudo shutdown -h now"
        return 0;
    }
    else {
        m_out < newline; 
        return 0;
    }
#else
    m_out << "(Testing mode - press [RETURN] to exit)\n";
    std::cin.ignore();
    return 0;
#endif
}