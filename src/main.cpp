#include <psp2/appmgr.h>
#include <psp2/apputil.h>
#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h> 
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/rtc.h>
#include <psp2/sysmodule.h>
#include <psp2/touch.h>
#include <psp2/power.h>
#include <psp2/kernel/rng.h> 
#include <psp2/kernel/clib.h>

#include "clib.h"
#include "../deps/libvita2d/include/vita2d.h"
#include "ftpvita.h"

#include <cstdlib>
#include <vector>
#include <deque>
#include <cstdarg>
#include <cstring>
#include <cmath>

#include "GDigitalPad.h"
#include "GDraw.h"
#include "GTimer.h"

using namespace std;

GTimer::Measure m;
bool launch_ftp = false;
bool doBreak = false;
char vita_ip[16] = {0};
unsigned short int vita_port = 1337;

int run = 1;

int app = 0;
int eboot = 0;
const char TITLE_ID[] = "GVITAV";
char LAUNCH_TITLE_ID[10] = {0};
static int showlog = 1;

typedef struct {
    float r;       // ∈ [0, 1]
    float g;       // ∈ [0, 1]
    float b;       // ∈ [0, 1]
} rgb;

typedef struct {
    float h;       // ∈ [0, 360]
    float s;       // ∈ [0, 1]
    float v;       // ∈ [0, 1]
} hsv;

void hsv2rgb(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV) {
  float fC = fV * fS; // Chroma
  float fHPrime = fmod(fH / 60.0, 6);
  float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
  float fM = fV - fC;
  
  if(0 <= fHPrime && fHPrime < 1) {
    fR = fC;
    fG = fX;
    fB = 0;
  } else if(1 <= fHPrime && fHPrime < 2) {
    fR = fX;
    fG = fC;
    fB = 0;
  } else if(2 <= fHPrime && fHPrime < 3) {
    fR = 0;
    fG = fC;
    fB = fX;
  } else if(3 <= fHPrime && fHPrime < 4) {
    fR = 0;
    fG = fX;
    fB = fC;
  } else if(4 <= fHPrime && fHPrime < 5) {
    fR = fX;
    fG = 0;
    fB = fC;
  } else if(5 <= fHPrime && fHPrime < 6) {
    fR = fC;
    fG = 0;
    fB = fX;
  } else {
    fR = 0;
    fG = 0;
    fB = 0;
  }
  
  fR += fM;
  fG += fM;
  fB += fM;
}

class GPage {
private:
    typedef void (*GPagedef)(void* arg);
    std::string title;
    GPagedef func = nullptr;
    void* arg = nullptr;
    int idx = 0;

public:
    GPage(const char* title, GPagedef func, void* arg = nullptr) :
        title(title), func(func), arg(arg)
    {
        this->title = title;
        this->func = func;
        this->arg = arg;
    }
    const std::string& getTitle() { return title; }
    GPagedef getFunc() { return func; }
    int getIndex() { return idx; }
    void setIndex(int idx) { this->idx = idx; }
};

class FTPLog {
    public:
    struct entry {
        std::string str;
        GColor col;
    };
    FTPLog() {
        fontHeight = 19;
        maxEntries = (440 / fontHeight) - 1;
    }

    void clear() {
        entries.clear();
    }

    void line(const char* logtext, ...)
    {
        SceDateTime time;
        sceRtcGetCurrentClockLocalTime(&time);

        char buf[2048] = "";
        va_list va_alist;
        va_start(va_alist, logtext);
        int ret = my_vsnprintf(buf, sizeof(buf), logtext, va_alist);
        va_end(va_alist);

        if (ret == -1)
            return;

        char buftime[32] = "";
        my_snprintf(buftime, 32, "[%02d:%02d:%02d.%06d]", sceRtcGetHour(&time), sceRtcGetMinute(&time), sceRtcGetSecond(&time), sceRtcGetMicrosecond(&time)); 

        std::string k = std::string(buftime) + std::string(buf);

        FILE *f = fopen("ux0:GFTP.log","a+");
        if(!f)
            return;

        fprintf(f, k.c_str());
        fprintf(f, "\n");
        fclose(f);
    }

    void add(const char* logtext, ...)
    {
        SceDateTime time;
        sceRtcGetCurrentClockLocalTime(&time);

        if (entries.size() > maxEntries)
           entries.pop_back();

        char buf[1000] = "";
        va_list va_alist;
        va_start(va_alist, logtext);
        int ret = my_vsnprintf(buf, sizeof(buf), logtext, va_alist);
        va_end(va_alist);

        if (ret == -1)
            return;

        char buftime[32] = "";
        my_snprintf(buftime, 32, "[%02d:%02d:%02d] ", sceRtcGetHour(&time), sceRtcGetMinute(&time), sceRtcGetSecond(&time));
        
        std::string e = buf;
        if(e.find_first_of('\t') != string::npos)
                e.erase(0, 1);

        static float i = 0;
        if(!i) {
            uint16_t k = 0;
            sceKernelGetRandomNumber(&k, 2);
            i = k % 360;
        }

        hsv H = { i, 1.0f, 1.0f };
        rgb R;
        hsv2rgb(R.r, R.g, R.b, H.h, H.s, H.v);
        GColor col = GColor(uint8_t(R.r*255.0f), uint8_t(R.g*255.0f), uint8_t(R.b*255.0f), 255);
        i += 3.0f;
        

        entry en;
        en.str = std::string(buftime) + e;
        en.col = col;

        entries.push_front(en);
    }

    void render()
    {
        int j = 0;
        for(auto &e : entries) {
            GFonts::text(GFonts::mainFont, 20, GFonts::left, GPoint(20, 500-j), e.col, e.str.c_str());
            j += fontHeight;
        }
    }

    private:
    std::deque<entry> entries;
    unsigned int maxEntries;
    int fontHeight;
};

FTPLog *g_pFTPLog = nullptr;

GPage pageHomeScreen("FTPVita", [](void* arg) -> void {
    (void)arg;

    GFonts::text(GFonts::mainFont, 20, GFonts::right, GPoint(950, 50), color_grey,
        "Running since %.2f min",
        double(m.now()) / 1000.0f / 1000.0f / 60.0f);

     GFonts::text(GFonts::mainFont, 20, GFonts::left, GPoint(10, 530), color_grey,
        "[ ] to exit, X to %s log, O to clear log, /\\ to turn screen off", showlog ? "hide" : "show");

    if(my_strlen(vita_ip)) 
        GFonts::text(GFonts::mainFont, 20, GFonts::left, GPoint(10, 50), color_white,
        "Listening on IP %s:%i\n", vita_ip, vita_port);

    if(logo)
        vita2d_draw_texture(logo, 960 / 2 - 64, 544 / 2 - 64);

    if(showlog)
        g_pFTPLog->render();

});

GPage pageNoWifi("No Wi-Fi", [](void* arg) -> void {
    (void)arg;

    GFonts::text(GFonts::mainFont, 20, GFonts::center, GPoint(960/2, 544/2-8), color_red, "You have to enable Wi-Fi");
    GFonts::text(GFonts::mainFont, 20, GFonts::center, GPoint(960/2, 544/2+8), color_red, "in order to run the server!");
});

static void info_log(const char *s)
{
    if(!s)
        return;

    if(showlog)
	    g_pFTPLog->add(s);

	char *p = 0;
	if ((p = my_strstr(s, TITLE_ID))) {
		app = 1;
		my_strncpy(LAUNCH_TITLE_ID, p, 9);
	}
	if(app && my_strstr(s, "STOR eboot.bin")) eboot = 1;
	if(eboot && my_strstr(s, "QUIT")) doBreak = 1;
}

namespace net {

    size_t curl_cb(char* d, size_t a, size_t b, string* c)
    {
        if (c && d && a && b)
            c->append(d, a * b);

        return a * b;
    }

    void init() //needed for curl which uses plain sockets, which are implemeted using sony's socket implementation, which requires you to to initialize the network
    {
        sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

        int size = 4 * 1024 * 1024;
        SceNetInitParam nip {
            malloc(size), size, 0
        };
        sceNetInit(&nip);
        sceNetCtlInit();
    }

    void release()
    {
        sceNetTerm();
        sceNetCtlTerm();
        sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
    }
};

class GPageManager {
private:
    vector<GPage*> pages;
    GPage* curPage = nullptr;
    int count = 0;

public:
    GPageManager() {}
    void add(GPage* page)
    {
        if (!page)
            return;

        page->setIndex(count++);
        pages.push_back(page);
    }
    void set(const string &title)
    {
        for (auto p : pages) {
            if (title == p->getTitle()) {
                curPage = p;
                break;
            }
        }
    }

    void set(unsigned int i)
    {
        if(i > pages.size())
            return;

        curPage = pages[i];
    }

    void next()
    {
        for (size_t i = 0; i < pages.size(); i++) {
            if (curPage->getIndex() == pages[i]->getIndex()) {
                if (i == (pages.size() - 1))
                    curPage = pages[0];
                else
                    curPage = pages[i + 1];

                break;
            }
        }
    }

    void prev()
    {
        for (size_t i = 0; i < pages.size(); i++) {
            if (curPage->getIndex() == pages[i]->getIndex()) {
                if (i == 0)
                    curPage = pages[pages.size() - 1];
                else
                    curPage = pages[i - 1];

                break;
            }
        }
    }

    void draw()
    { // 960x544
        if (curPage) {
            GFonts::text(GFonts::mainFont, 20, GFonts::center, GPoint(960 / 2, 20),
                color_red, "%s", curPage->getTitle().c_str());

            SceDateTime time;
            sceRtcGetCurrentClockLocalTime(&time);

            auto bat = scePowerGetBatteryLifePercent();

            GFonts::text(GFonts::mainFont, 20, GFonts::right, GPoint(960, 20), color_white,
                "%s%d%%  %02d.%02d.%02d  %02d:%02d:%02d.%01d", 
                scePowerIsBatteryCharging() ? "*" : "",bat, sceRtcGetDay(&time),
                sceRtcGetMonth(&time), sceRtcGetYear(&time) - 2000, 
                sceRtcGetHour(&time), sceRtcGetMinute(&time), sceRtcGetSecond(&time),
                sceRtcGetMicrosecond(&time) / 100000
            );

            GFonts::text(GFonts::mainFont, 20, GFonts::left, GPoint(10, 20), color_white, "%s", "by xerpi & Gordon");

            g_pDraw->line(GPoint(0, 25), GPoint(960, 25), color_red, 2);

            auto func = curPage->getFunc();
            func(nullptr);
        }
    }
};

GPageManager* g_pPageMgr = new GPageManager();

void buttonPress(uint32_t key, uint64_t startPressTime) {
    (void)key;
    (void)startPressTime;
}

void buttonRelease(uint32_t key, uint64_t endPressTime, uint64_t duration)
{
    (void)endPressTime;
    (void)duration;

    if (key == SCE_CTRL_SQUARE) {
        doBreak = true;
    }

    if (key == SCE_CTRL_CROSS) {
        showlog = !showlog;
    }

    if (key == SCE_CTRL_CIRCLE) {
        g_pFTPLog->clear();
    }

    if(key == SCE_CTRL_TRIANGLE) {
        scePowerRequestDisplayOff();
    }

    if (key == SCE_CTRL_SELECT && duration > 1500000) {
        scePowerRequestColdReset();
    }
}

void draw()
{
    g_pPageMgr->draw();
    g_pDigitalPad->ctrlHandler();
}

void gfx_render()
{
    vita2d_start_drawing();
    vita2d_clear_screen();        
    draw();
    vita2d_end_drawing();
    vita2d_swap_buffers();
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    m.start();

    auto tid = sceKernelCreateThread("anti-suspend-thread", [] (unsigned int a, void *b) -> int {
        for(;;) {
            sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND);
            //sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_OLED_OFF);
            //sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_OLED_DIMMING);
            sceKernelDelayThread(250000);
            if(doBreak)
                break;
        }
        sceKernelExitThread(0);
        return 0;
    }, 0x10000100, 0x10000, 0, 0, nullptr);
    sceKernelStartThread(tid, 0, nullptr);

	SceCtrlData pad;
	SceAppUtilInitParam init_param;
	SceAppUtilBootParam boot_param;

    my_memset(&init_param, 0, sizeof(SceAppUtilInitParam));
	my_memset(&boot_param, 0, sizeof(SceAppUtilBootParam));
	sceAppUtilInit(&init_param, &boot_param);

    ftpvita_set_info_log_cb(info_log);

    net::init();
    
    vita2d_set_clear_color(0xFF000000);
    vita2d_set_vblank_wait(0);
    vita2d_init();

    g_pFTPLog = new FTPLog();
    g_pDraw = new GDraw();
    g_pDigitalPad = new GDigitalPad();


    g_pDraw->init();
    g_pDigitalPad->init();
    GFonts::create();

    g_pPageMgr->add(&pageHomeScreen);
    g_pPageMgr->add(&pageNoWifi);

    int state = 0;
    sceNetCtlInetGetState(&state);

    while (state != 3 || ftpvita_init(vita_ip, &vita_port) < 0) {
		sceNetCtlInetGetState(&state);
        g_pPageMgr->set("No Wi-Fi");
		gfx_render();
        sceKernelDelayThread(83333);
    }

    g_pPageMgr->set("FTPVita");

    ftpvita_add_device("app0:");
	ftpvita_add_device("ux0:");
	ftpvita_add_device("ur0:");
	ftpvita_add_device("uma0:");
	ftpvita_add_device("imc0:");
	ftpvita_add_device("pd0:");
	ftpvita_add_device("sa0:");
	ftpvita_add_device("tm0:");
	ftpvita_add_device("ud0:");
	ftpvita_add_device("vs0:");
	ftpvita_add_device("vd0:");
	ftpvita_add_device("xmc0:");
	ftpvita_add_device("gro0:");
	ftpvita_add_device("grw0:");
	ftpvita_add_device("hist0:");
	ftpvita_add_device("tty");

	if (sceAppUtilMusicMount() == 0)
		ftpvita_add_device("music0:");
	if (sceAppUtilPhotoMount() == 0)
		ftpvita_add_device("photo0:");

    do {
        auto start = GTimer::GetTickCount();
        if (doBreak)
            break;
        auto end = GTimer::GetTickCount();
        gfx_render();
        
        sceKernelDelayThread(83333);
    } while (true);

    vita2d_fini();
    delete g_pDraw;
    delete g_pFTPLog;
    GFonts::release();
    ftpvita_fini();
    net::release();
    sceAppUtilPhotoUmount();
	sceAppUtilMusicUmount();
    sceAppUtilShutdown();

    if (app && eboot)
	{
		char uri[64] = "";
		my_snprintf(uri, 64, "psgm:play?titleid=%s", LAUNCH_TITLE_ID);
		sceAppMgrLaunchAppByUri(0x20000, uri);
		sceKernelDelayThread(0);
    }

    sceKernelExitProcess(0);
    return 0;
}