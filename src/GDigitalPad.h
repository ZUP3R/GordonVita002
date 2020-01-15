#ifndef __GDIGITALPAD__
#define __GDIGITALPAD__

#include <psp2/ctrl.h>

#include <unordered_map>
#include <vector>

struct button_map {
    std::string name;
    uint64_t startPressTime, endPressTime;
    int state;
};

typedef std::unordered_map<uint32_t, button_map> pad_map;
static pad_map padMap;

class GDigitalPadMap {
private:
    GDigitalPadMap() {}

    static void insert(const std::string &buttonName, uint32_t keyValue)
    {
        padMap.insert({ keyValue, { buttonName, 0, 0, 0 } });
    }
    static pad_map& init()
    {
        static bool _init = false;
        if (!_init) {
            insert("SELECT", SCE_CTRL_SELECT);
            insert("L3", SCE_CTRL_L3);
            insert("R3", SCE_CTRL_R3);
            insert("START", SCE_CTRL_START);
            insert("UP", SCE_CTRL_UP);
            insert("RIGHT", SCE_CTRL_RIGHT);
            insert("DOWN", SCE_CTRL_DOWN);
            insert("LEFT", SCE_CTRL_LEFT);
            insert("LTRIGGER", SCE_CTRL_LTRIGGER);
            insert("RTRIGGER", SCE_CTRL_RTRIGGER);
            insert("L1", SCE_CTRL_L1);
            insert("R1", SCE_CTRL_R1);
            insert("TRIANGLE", SCE_CTRL_TRIANGLE);
            insert("CIRCLE", SCE_CTRL_CIRCLE);
            insert("CROSS", SCE_CTRL_CROSS);
            insert("SQUARE", SCE_CTRL_SQUARE);
            insert("PSBUTTON", SCE_CTRL_PSBUTTON);
            insert("VOLUP", SCE_CTRL_VOLUP);
            insert("VOLDOWN", SCE_CTRL_VOLDOWN);
            insert("POWER", SCE_CTRL_POWER);
    
            _init = true;
        }
        return padMap;
    }

public:

    static pad_map& list() { return init(); }
    static uint32_t get(const char* strButton)
    {
        for (auto it : list())
            if (it.second.name == std::string(strButton))
                return it.first;
        return 0;
    }
    static button_map& get(uint32_t buttonKeyValue)
    {
        return list()[buttonKeyValue];
    }
};

class GDigitalPad {
public:
    typedef void (*notify_t)(int code);
    struct event_t {
        notify_t cb;
        int arg; 
    };

    GDigitalPad();
    ~GDigitalPad();

    void init();
    void execHandler(int code);
    void ctrlHandler();
    void addNotifyProcedure(int code = 0, notify_t func = 0);

private:
    SceCtrlData pad;
    SceCtrlData oldPad;
    std::vector<event_t> eventNotifyList;
};

extern GDigitalPad *g_pDigitalPad;
extern void buttonPress(uint32_t key, uint64_t startPressTime);
extern void buttonRelease(uint32_t key, uint64_t endPressTime, uint64_t duration);

#endif // !__GDIGITALPAD__