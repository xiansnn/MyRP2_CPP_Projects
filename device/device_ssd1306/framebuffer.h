#if !defined(FRAMEBUFFER_H)
#define FRAMEBUFFER_H

#include "pico/stdlib.h"
#include <string>

enum class Framebuffer_format
{
    MONO_VLSB,
    MONO_HLSB,
    MONO_HMSB,
    RGB565
};
enum class Framebuffer_color
{
    black = 0,
    white = 1
};

class Framebuffer
{
private:
    /* data */
    size_t frame_width;
    size_t frame_height;
    size_t buffer_size;
    Framebuffer_format format;
public:
    uint8_t * buffer;

    Framebuffer(uint8_t buffer[], size_t width, size_t height, Framebuffer_format format);
    void fill(Framebuffer_color c);
    void pixel(uint8_t x, uint8_t y, Framebuffer_color c = Framebuffer_color::white);
    void hline(uint8_t x, uint8_t y, size_t w, Framebuffer_color c= Framebuffer_color::white);
    void vline(uint8_t x, uint8_t y, size_t h, Framebuffer_color c= Framebuffer_color::white);
    void line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, Framebuffer_color c= Framebuffer_color::white);
    void rect(uint8_t x, uint8_t y, size_t w, size_t h, bool fill=false, Framebuffer_color c= Framebuffer_color::white);
    void ellipse(uint8_t x, uint8_t y, uint8_t xr, uint8_t yr, bool fill=false, uint8_t m= 0b1111, Framebuffer_color c= Framebuffer_color::white);
    void text(std::string s, uint8_t x, uint8_t y, Framebuffer_color c= Framebuffer_color::white);
};

#endif // FRAMEBUFFER_H