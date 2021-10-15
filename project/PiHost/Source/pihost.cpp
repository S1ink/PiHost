#include <iostream>
#include <pigpio.h>

#include "tasks.h"
#include "parse.h"

#include "output.h"
#include "timing.h"
#include "basic.h"
#include "sighandle.h"
#include "program.h"
#include "resources.h"

#include "PiLib/System/syscom.h"
#include "PiLib/System/stats.h"
#include "PiLib/IO/gpio.h"

CE_STR version = "1.3.6b";

//all compatible thread modules must be in the from of ~ typedef void(*)(const char*, std::ostream&)
namespace thmods {
    void aptRun(const char* message, const olstream& logs) {
        olstream out(logs);
        const char* sep = "*******************************************************************";
        StopWatch ptime("Total elapsed time", &out, 0);
        ((((out <<= sep) <= "\nPihost internal APT updater initialized. (") <= dateStamp()) <= ")\n") <= sep;

        ((out <= "\n\nUpdating Repos:\n") <= sep) <= newline;
        int updates = pilib::aptUpdate(out.open());

        out <= "\nUpgrading:\n" <= sep <= newline;
        int upgrades = pilib::aptUpgrade(out.open());

        out <= "\nCleaning... ";
        system("sudo apt clean");
        out <= "Finished cleaning.\n\n"
            <= sep <= "\nProcess finished at: " <= dateStamp() 
            <= "\nUpdates: " <= updates <= "\nUpgrades: " <= upgrades < newline;
        ptime.end();
    }

    void winBackup(const char* message, const olstream& logs) {
        olstream out(logs);
        const char* sep = "*****************************************************************";
        StopWatch ptime("Total elapsed time", &out, 0);
        (((((out <<= sep) <= "\nPihost internal WinBackup initialized. (") <= dateStamp()) <= ")\n") <= sep) <= "\n\n";

        winSync(message, out.open());

        out <= sep <= "\nProcess finished at: " <= dateStamp() < newline;
        ptime.end();
    }
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

//add argument support for paths and intervals
int main(int argc, char* argv[]) {
    progdir.setDir(argv[0]);

    time_t update_intv = 10;
#ifndef TESTING
    uint fan_pin = gpin::pi_fan;
    uint button_pin = gpin::pi_power;
    float fan_speed = 40.f;
#endif
    int warn_temp = 40;
#ifndef TESTING
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
#endif
            {"warningtemp", &warn_temp},
            {"pollinterval", &update_intv},
#ifndef TESTING
            {"output", &main_output},
#endif
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
        { {"apt", thmods::aptRun}, {"winbackup", thmods::winBackup}, },
        m_out,
        update_intv
    );
    tasks.launch();

    std::this_thread::sleep_for(CHRONO::seconds(1));
    (((m_out <<= dateStamp()) <= " : ") <= tasks.getThreads()) < " task threads started\n";

    //TODO: PID loop
    float temp;
    while (run) {
        temp = pilib::sys::cpuTemp();
        if (temp >= warn_temp) {
            (((((m_out <<= dateStamp()) <= " : High SoC temp of ") <= temp) <= "*C - {CPU%:") <= pilib::sys::cpuPercent(CHRONO::seconds(1))) < "}\n";
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
    return 0;
#endif
}