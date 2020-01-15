#include "GTimer.h"
#include <psp2/rtc.h>

GTimer::Measure::Measure(bool startnow)
{
    _start = 0;
    _stop = 0;
    if(startnow)
        start();
}

void GTimer::Measure::start()
{
    _start = GTimer::GetTickCount();
}

void GTimer::Measure::reset()
{
    Measure();
    start();
}

void GTimer::Measure::stop()
{
    _stop = GetTickCount();
}

uint64_t GTimer::Measure::now()
{
    if (_start == 0)
        return 0;

    return GTimer::GetTickCount() - _start;
}

uint64_t GTimer::Measure::elapsedStopTime()
{
    return _stop - _start;
}

uint64_t GTimer::Measure::initialTime()
{
    return _start;
}

uint64_t GTimer::Measure::stopTime()
{
    return _stop;
}

uint64_t GTimer::GetTickCount()
{
    SceRtcTick tick{ 0 };
    sceRtcGetCurrentTick(&tick);

    return tick.tick;
}

GTimer::GTimer(const uint64_t &value)
{
    _usec = value;
    _started = false;
    _stopped = false;
}

void GTimer::start()
{
    _started = true;
    _stopped = false;

    m.start();
}

bool GTimer::started()
{
    return _started;
}

bool GTimer::finished()
{
    if (_started) {
        if (!running()) {
            _started = false;
            return true;
        }
    }
    return false;
}

bool GTimer::running()
{
    if (_usec >= m.now())
        return true;

    return false;
}

void GTimer::restart()
{
    m.stop();
    start();
}

void GTimer::stop()
{
    m.stop();
    _started = false;
    _stopped = true;
}