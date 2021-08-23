#include "gpio.h"

namespace pilib{
    int getStatus() {
        return gpioRead(gpin::pc_status);
    }

    int getSwitch() {
        return gpioRead(gpin::pi_power);
    }

    void activateSwitch(int gpiodevice) {
        gpioWrite(gpiodevice, 1);
        gpioDelay(250000);
        gpioWrite(gpiodevice, 0);
    }

    void setNoctua(float percent) {
        int speed = 10000 * percent;
        gpioHardwarePWM(gpin::pi_fan, 25000, speed);
    }

    void init(float fanspeed) {
        gpioInitialise();
        gpioSetMode(gpin::pc_power, PI_OUTPUT);
        gpioSetMode(gpin::pc_reset, PI_OUTPUT);
        gpioSetMode(gpin::pc_status, PI_INPUT);
        gpioSetMode(gpin::pi_power, PI_INPUT);
        gpioHardwarePWM(gpin::pi_fan, 25000, ((int)(fanspeed * 10000)));
        return;
    }
}