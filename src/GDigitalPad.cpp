#include "GDigitalPad.h"
#include "GTimer.h"
#include <psp2/rtc.h>
#include <cstring>
#include <vector>

GDigitalPad *g_pDigitalPad = nullptr;

GDigitalPad::GDigitalPad()
{
}

GDigitalPad::~GDigitalPad()
{
}

void GDigitalPad::init()
{
    memset(&pad, 0, sizeof(pad));
    memset(&oldPad, 0, sizeof(oldPad));
}

void GDigitalPad::addNotifyProcedure(int code, GDigitalPad::notify_t func)
{
    if(!func)
        return;

    eventNotifyList.push_back({func, code});
}

void GDigitalPad::execHandler(int code) {
    for(auto list: eventNotifyList) {
        if(list.arg == code)
            list.cb((uint64_t)list.arg);
    }
}

void GDigitalPad::ctrlHandler()
{
    memcpy(&oldPad, &pad, sizeof(pad));
    memset(&pad, 0, sizeof(pad));
    /*auto retVal = */sceCtrlPeekBufferPositive(0, &pad, 1);
  /*  if (!GInputManager::didSuccess(retVal))
        return;
*/
    auto changed = pad.buttons ^ oldPad.buttons;

    if (!changed)
        return;

    for (auto& it : GDigitalPadMap::list()) {
        if (changed & it.first) {
            it.second.state = !it.second.state;

            if (it.second.state) {
                it.second.startPressTime = GTimer::GetTickCount();
                buttonPress(it.first, it.second.startPressTime);
                execHandler(1);
            } else {
                it.second.endPressTime = GTimer::GetTickCount();
                auto diff = it.second.endPressTime - it.second.startPressTime;
                buttonRelease(it.first, it.second.endPressTime, diff);
                execHandler(2);
            }
        }
    }
}