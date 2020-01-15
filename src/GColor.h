#ifndef __GCOLOR__
#define __GCOLOR__

#include <cinttypes>

class GColor {
public:
    //GColor();
    GColor(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255);
    GColor(const GColor& color);
    GColor& operator=(const GColor& color);
    uint32_t get() const;

    uint8_t r=0, g=0, b=0, a=0;

private:
    uint32_t color = 0;
    bool isOk = false;

protected:
    void set();
};

extern GColor color_red;
extern GColor color_green;
extern GColor color_blue;
extern GColor color_white;
extern GColor color_black;
extern GColor color_yellow;
extern GColor color_grey;

#endif
