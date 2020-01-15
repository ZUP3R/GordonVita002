#include "clib.h"
#include "GDraw.h"
#include "fonts/Iceland.h"

#include <psp2/kernel/clib.h> 

#include <cstdio>
#include <cstdarg>

vita2d_texture *logo = nullptr;

namespace GFonts {
    std::list<GFont*> loaded;
    GFont *mainFont = nullptr;
    vita2d_pgf *pgfFont = nullptr;
    
    GFont *buildFontMem(const void* ttf, const size_t ttf_size)
    {
        GFont *ptr = vita2d_load_font_mem(ttf, ttf_size);
        if(!ptr)
            return nullptr;

        loaded.push_back(ptr);
        return ptr;
    }

    void releaseFont(GFont *f)
    {
        if(!f)
            return;

        vita2d_free_font(f);
        loaded.remove(f);
        f = nullptr;
    }

    void create() 
    {
        pgfFont = vita2d_load_custom_pgf("sa0:data/font/pgf/ltn4.pgf"); //4 8 10 12
        //if(!pgfFont)
            mainFont = buildFontMem(ICELAND, SIZE_ICELAND);
    }

    void release()
    {
        vita2d_free_pgf(pgfFont);
        for(auto f: loaded) {
            vita2d_free_font(f);
        }
    }

    int text(GFont *f, int size, ALIGN align, const GPoint& pt, const GColor& color, const char *format, ...)
     {
          if(!f)
            return 0;

        char buf[1024] = { 0 };
        va_list va_alist;
        va_start(va_alist, format);
        int ret = my_vsnprintf(buf, sizeof(buf), format, va_alist);
        va_end(va_alist);

        if (ret == -1)
            return 0;

        int width = 0, height = 0;
        vita2d_font_text_dimensions(f, size, buf, &width, &height);
        GPoint p(pt.x, pt.y);

        if(align == ALIGN::left) 
            ;
        else if(align == ALIGN::center)
            p.x -= width / 2;
        else if(align == ALIGN::right)
            p.x -= width;

        return vita2d_font_draw_text(f, p.x, p.y, color.get(), size, buf);
     }

      int text(vita2d_pgf *f, float size, ALIGN align, const GPoint& pt, const GColor& color, const char *format, ...)
     {
          if(!f)
            return 0;

        char buf[1024] = { 0 };
        va_list va_alist;
        va_start(va_alist, format);
        int ret = my_vsnprintf(buf, sizeof(buf), format, va_alist);
        va_end(va_alist);

        if (ret == -1)
            return 0;

        int width = 0, height = 0;
        vita2d_pgf_text_dimensions(f, size, buf, &width, &height);
        GPoint p(pt.x, pt.y);

        if(align == ALIGN::left) 
            ;
        else if(align == ALIGN::center)
            p.x -= width / 2;
        else if(align == ALIGN::right)
            p.x -= width;

        return vita2d_pgf_draw_text(f, p.x, p.y, color.get(), size, buf);
     }
 };

GDraw* g_pDraw = nullptr;

GDraw::GDraw()
{
    
}

GDraw::~GDraw()
{
    release();
}

void GDraw::init()
{
    logo = vita2d_load_PNG_file("app0:sce_sys/icon0.png");
}

void GDraw::release()
{
    vita2d_free_texture(logo);
}

void GDraw::pixel(const GPoint& pt, const GColor& color)
{
    vita2d_draw_pixel(pt.x, pt.y, color.get());
}

void GDraw::line(const GPoint& pt1, const GPoint& pt2, const GColor& color, int linewidth)
{
    vita2d_draw_line_lw(pt1.x, pt1.y, pt2.x, pt2.y, color.get(), linewidth);
}

void GDraw::quad2(const GPoint& pt, const GSize& sz, const GColor& color, int linewidth)
{
    vita2d_draw_rectangle_lw(pt.x, pt.y, sz.w, sz.h, color.get(), linewidth);
}

void GDraw::filledQuad(const GPoint& pt, const GSize& sz, const GColor& color)
{
    vita2d_draw_fill_rectangle(pt.x, pt.y, sz.width, sz.height, color.get());
}

void GDraw::circle(const GPoint& pt, const GColor& color, int radius, int linewidth)
{
    vita2d_draw_circle_lw(pt.x, pt.y, radius, color.get(), linewidth);
}

void GDraw::filledCircle(const GPoint& pt, const GColor& color, int radius)
{
    vita2d_draw_fill_circle(pt.x, pt.y, radius, color.get());
}

void GDraw::cross(const GPoint& pt, const GColor& color, int cross_radius)
{
    int fix = cross_radius == 2 ? 1 : cross_radius % 2;

    line(GPoint(pt.x - cross_radius, pt.y - cross_radius), GPoint(pt.x + cross_radius + fix, pt.y + cross_radius + fix), color, 1);
    line(GPoint(pt.x + cross_radius, pt.y - cross_radius), GPoint(pt.x - cross_radius - fix, pt.y + cross_radius + fix), color, 1);
}

void GDraw::circleCross(const GPoint& pt, const GColor& color, int radius)
{
    filledCircle(pt, color, radius);
    cross(pt, color, radius + 2);
}
