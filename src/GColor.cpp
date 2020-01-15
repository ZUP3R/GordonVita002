#include "GColor.h"

GColor color_red(255, 0, 0, 255);
GColor color_green(0, 255, 0, 255);
GColor color_blue(0, 0, 255, 255);
GColor color_white(255, 255, 255, 255);
GColor color_black(0, 0, 0, 255);
GColor color_yellow(255, 255, 0, 255);
GColor color_grey(192, 192, 192, 255);

GColor::GColor(uint8_t r, uint8_t g, uint8_t b,
    uint8_t a)
{

    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
    set();
    isOk = true;
}

GColor::GColor(const GColor& color)
{
    r = color.r;
    g = color.g;
    b = color.g;
    a = color.b;
    this->color = color.color;
    isOk = color.isOk;
}

GColor& GColor::operator=(const GColor& color)
{
    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;

    set();

    return *this;
}

void GColor::set()
{
    //color = ((((a))<<24) | (((b))<<16) | (((g))<<8) | (((r))<<0));
    color = 0;
    color |= r << 0;
    color |= g << 8;
    color |= b << 16;
    color |= a << 24;
}

uint32_t GColor::get() const
{
    return color;
}
