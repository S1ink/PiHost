#include "headers/timing.h"

namespace pilib {
    time_t now() {
        return CHRONO::system_clock::to_time_t(CHRONO::system_clock::now());
    }

    tm* struct_now() {
        time_t n = CHRONO::system_clock::to_time_t(CHRONO::system_clock::now());
        return localtime(&n);
    }

    double perfTimer(CHRONO::time_point<CHRONO::high_resolution_clock>& start) {
        CHRONO::duration<double> diff = CHRONO::high_resolution_clock::now() - start;
        return diff.count();
    }

    StopWatch::StopWatch(bool now) {
        if (now) {
            ref = CHRONO::high_resolution_clock::now();
        }
    }

    void StopWatch::setStart() {
        ref = CHRONO::high_resolution_clock::now();
    }

    double StopWatch::getDuration() {
        CHRONO::duration<double> diff = CHRONO::high_resolution_clock::now() - ref;
        return diff.count();
    }

    void StopWatch::pTotal(std::ostream& output) {
        CHRONO::duration<double> diff = CHRONO::high_resolution_clock::now() - ref;
        output << "Total elapsed time: " << diff.count() << " seconds" << newline;
    }

    StopWatch::~StopWatch(){
        CHRONO::duration<double> diff = CHRONO::high_resolution_clock::now() - ref;
        std::cout << "Total elapsed time: " << diff.count() << " seconds" << newline;
    }

    time_d createTOD(uint16_t hr, uint16_t min, uint16_t sec) {
        return (hr * 3600) + (min * 60) + (sec);
    }

    time_d DayTime::toTOD() {
        return (hr * 3600) + (min * 60) + (sec);
    }

    time_t d_untilNext(const DayTime tme) {
        CHRONO::time_point<CHRONO::system_clock> now = CHRONO::system_clock::now();
        time_t tt = CHRONO::system_clock::to_time_t(now);
        tm t = *localtime(&tt);
        if (t.tm_hour > tme.hr || (t.tm_hour == tme.hr && t.tm_min > tme.min) || (t.tm_hour == tme.hr && t.tm_min == tme.min && t.tm_sec > tme.sec)) {
            tt += 86400;
            t = *localtime(&tt);
        }
        t.tm_sec = tme.sec;
        t.tm_min = tme.min;
        t.tm_hour = tme.hr;
        CHRONO::duration<time_t> diff = CHRONO::duration_cast<CHRONO::seconds>(CHRONO::system_clock::from_time_t(mktime(&t)) - now);
        return diff.count() + 1;
    }

    time_t d_untilNext(DayTime& tme) {
        CHRONO::time_point<CHRONO::system_clock> now = CHRONO::system_clock::now();
        time_t tt = CHRONO::system_clock::to_time_t(now);
        tm t = *localtime(&tt);
        if (t.tm_hour > tme.hr || (t.tm_hour == tme.hr && t.tm_min > tme.min) || (t.tm_hour == tme.hr && t.tm_min == tme.min && t.tm_sec > tme.sec)) {
            tt += 86400;
            t = *localtime(&tt);
        }
        t.tm_sec = tme.sec;
        t.tm_min = tme.min;
        t.tm_hour = tme.hr;
        CHRONO::duration<time_t> diff = CHRONO::duration_cast<CHRONO::seconds>(CHRONO::system_clock::from_time_t(mktime(&t)) - now);
        return diff.count() + 1;
    }

    CHRONO::time_point<CHRONO::system_clock> d_nextTime(const DayTime tme) {
        time_t tt = CHRONO::system_clock::to_time_t(CHRONO::system_clock::now());
        tm t = *localtime(&tt);
        if (t.tm_hour > tme.hr || (t.tm_hour == tme.hr && t.tm_min > tme.min) || (t.tm_hour == tme.hr && t.tm_min == tme.min && t.tm_sec > tme.sec)) {
            tt += 86400;
            t = *localtime(&tt);
        }
        t.tm_sec = tme.sec;
        t.tm_min = tme.min;
        t.tm_hour = tme.hr;
        return CHRONO::system_clock::from_time_t(mktime(&t));
    }

    CHRONO::time_point<CHRONO::system_clock> d_nextTime(DayTime& tme) {
        time_t tt = CHRONO::system_clock::to_time_t(CHRONO::system_clock::now());
        tm t = *localtime(&tt);
        if (t.tm_hour > tme.hr || (t.tm_hour == tme.hr && t.tm_min > tme.min) || (t.tm_hour == tme.hr && t.tm_min == tme.min && t.tm_sec > tme.sec)) {
            tt += 86400;
            t = *localtime(&tt);
        }
        t.tm_sec = tme.sec;
        t.tm_min = tme.min;
        t.tm_hour = tme.hr;
        return CHRONO::system_clock::from_time_t(mktime(&t));
    }
}