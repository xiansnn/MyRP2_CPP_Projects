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
    uint8_t * buffer;
    size_t frame_width;
    size_t frame_height;
public:
    Framebuffer(uint8_t buffer[], size_t width, size_t height, Framebuffer_format format);
    void fill(Framebuffer_color c);
    void pixel(uint8_t x, uint8_t y, Framebuffer_color c);
    void hline(uint8_t x, uint8_t y, Framebuffer_color c);
    void vline(uint8_t x, uint8_t y, Framebuffer_color c);
    void line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, Framebuffer_color c);
    void rect(uint8_t x, uint8_t y, size_t w, size_t h, Framebuffer_color c, bool fill);
    void ellipse(uint8_t x, uint8_t y, uint8_t xr, uint8_t yr, Framebuffer_color c, bool fill, uint8_t m);
    void text(std::string s, uint8_t x, uint8_t y, Framebuffer_color c);
};
