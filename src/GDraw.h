#ifndef __GDRAW__
#define __GDRAW__

#include "GColor.h"
#include "GPoint.h"

#include <vita2d.h>
#include <list>

enum class pgf_textalign : uint8_t {
    LEFT,
    CENTER,
    RIGHT
};

typedef vita2d_font GFont;

namespace GFonts {
    enum ALIGN {
    left, center, right };

    extern std::list<GFont*> loaded;
    extern GFont* mainFont;
    extern GFont* consoleFont;
    extern GFont* clockFont;
    extern vita2d_pgf *pgfFont;
    
    extern GFont *buildFontMem(const void* ttf, const size_t ttf_size);

    extern void releaseFont(GFont *f);
    extern void create();
    void release();

    extern int text(GFont *f, int size, ALIGN align, const GPoint& pt, const GColor& color, const char *format, ...);
    extern int text(vita2d_pgf *f, float size, ALIGN align, const GPoint& pt, const GColor& color, const char *format, ...);
 };

class GDraw {
public:
    GDraw();
    ~GDraw();

    void init();
    void release();

    void pixel(const GPoint& pt, const GColor& color);
    void line(const GPoint& pt1, const GPoint& pt2, const GColor& color, int linewidth = 1);
    void quad2(const GPoint& pt, const GSize& sz, const GColor& color, int linewidth = 1);
    void filledQuad(const GPoint& pt, const GSize& sz, const GColor& color);
    void circle(const GPoint& pt, const GColor& color, int radius, int linewidth = 1);
    void filledCircle(const GPoint& pt, const GColor& color, int radius);
    void cross(const GPoint& pt, const GColor& color, int cross_radius);
    void circleCross(const GPoint& pt, const GColor& color, int radius = 4);
    //void textPgf(const GPoint& pt, float scale, pgf_textalign flags, const GColor& color, const char* format, ...);

private:
    //vita2d_pgf* pgf = nullptr;
};

extern GDraw* g_pDraw;
extern vita2d_texture *logo;

#endif
