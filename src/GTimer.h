#ifndef __GTIMER__
#define __GTIMER__

#include <cinttypes>
#include <string.h>

class GTimer {
public:
    GTimer(const uint64_t &value);

    void start();
    void stop();
    bool finished();
    bool running();
    void restart();

    bool started();
    bool _started;
    bool _stopped;

    static uint64_t GetTickCount();

    class Measure {
    private:
        uint64_t _start;
        uint64_t _stop;

    public:
        Measure(bool startnow = true);
        void start();
        void stop();
        void reset();
        uint64_t now();
        uint64_t elapsedStopTime();
        uint64_t initialTime();
        uint64_t stopTime();
    };
    Measure m;

private:
    uint64_t _usec;
};

#endif
