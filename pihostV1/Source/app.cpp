#include "pilib.h"
#include "app.h"

void gpio1() {
    int var;
    gpio::init();
    printf("Fan started\n");
    while (true) {
        printf("Enter a number: ");
        std::cin >> var;
        if (var == 1) {
            gpio::activateSwitch(p_power);
            printf("Activated power switch\n");
        }
        else if (var == 0) {
            std::cout << gpio::getStatus() << std::endl;
        }
        else if (var == -1) {
            break;
        }
    }
    gpioTerminate();
    return;
}

void gpio2() {
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    gpio::init();
    gpioSetISRFunc(p_switch, RISING_EDGE, 0, callback);
    std::thread itr(input_);
    std::cout << newline << " * * * Loop started, enter 'q' to exit * * * " << newline << newline;
    while (run) {
        std::cout << info::dateStamp() << " - CPU temp: " << info::cputemp() << newline;
        std::cout << info::dateStamp() << " - GPU temp: " << info::gputemp() << newline;
        std::cout << "gpio pc status: " << gpio::getStatus() << newline;
        std::cout << "gpio button status: " << gpio::getSwitch() << newline;
        std::cout << info::elapsed_time(start) << " seconds have elapsed" << newline << newline;
        int wait = (1 - (info::elapsed_time(start) - (int)info::elapsed_time(start))) * 1000000;
        usleep(wait);
    }
    itr.join();
    gpioTerminate();
    std::cout << "Total elapsed time: " << info::elapsed_time(start) << " seconds" << newline;
}

int gpioDebugMode(bool dinit) {
    std::chrono::time_point<std::chrono::system_clock> fstart = std::chrono::system_clock::now();
    int ret;
    int inp;
    bool exit = false;
    if (dinit) {
        gpio::init();
    }
    db::ptitle();
    while (!exit) {
        db::pmodes();
        inp = db::_input();
        switch (inp) {
        case 0:
            exit = true;
            break;
        case 1:
            db::ePins();
            break;
        case 2:
            db::vPins();
            break;
        case 3:
            db::ePWM();
            break;
        case 4:
            db::sMacros();
            break;
        default:
            std::cout << "Error, try again" << newline;
            break;
        }
    }
    gpioTerminate();
    return ret;
}

namespace db {
    void ptitle() {
        std::cout << " * * * GPIO Debug Mode * * * " << newline;
    }

    void pmodes() {
        std::cout << "Modes:" << newline << " - Edit Pinmodes: 1" << newline << " - View Pin States : 2" << newline << " - Change PWM Speed: 3" << newline << " - Macros Menu: 4" << newline << " - Exit: 0" << newline;
    }

    int _input(std::string desc) {
        int ret;
        std::cout << desc;
        std::cin >> ret;
        std::cout << newline;
        return ret;
    }

    void ePins() {
        std::cout << " * * * GPIO Pin Editing Mode * * * " << newline;
        bool crun = true;
        while (crun) {
            int pn = db::_input("Type a pin number (GPIO format): ");
            std::cout << "Pin Modes:" << newline << " - Input: 0" << newline << " - Output: 1" << newline << " - PWM: 2" << newline;
            int mn = db::_input("Type a mode: ");
            if (mn == 3) {
                int speed = db::_input("Enter a PWM speed (0-255): ");
                gpioPWM(pn, speed);
            }
            else {
                gpioSetMode(pn, mn);
            }
            std::cout << newline << "GPIO Pin " << pn << " set > What would you like to do?:" << newline << " - Return to menu: 0" << newline << " - Edit another pin: 1" << newline;
            crun = db::_input();
        }
    }

    void vPins() {
        std::cout << " * * * GPIO Pin Viewing Mode * * * " << newline;
        std::cout << newline << "Pin viewing mode is currently not available" << newline << "Returning to main menu..." << newline;
    }

    void ePWM() {
        std::cout << " * * * PWM Speed Editing Mode * * * " << newline;
        bool crun = true;
        while (crun) {
            int sp = db::_input("Type a speed (percentage): ");
            //make sure input is safe
            gpioHardwarePWM(p_fan, 25000, (10000 * sp));
            std::cout << newline << "Fan set to " << sp << "% > what would you like to do?:" << newline << " - Return to menu: 0" << newline << " - Change speed again: 1" << newline;
            crun = db::_input();
        }
    }

    void sMacros() {
        std::cout << " * * * Macro Selection Mode * * * " << newline;
        std::cout << newline << "Macro selection is currently not available" << newline << "Returning to main menu..." << newline;
    }
}