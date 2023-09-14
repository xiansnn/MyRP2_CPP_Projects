#include "framebuffer.h"

Framebuffer::Framebuffer(uint8_t buffer[], size_t width, size_t height, Framebuffer_format format)
{
    assert( format == Framebuffer_format::MONO_VLSB);
    this->buffer = buffer;
    this->frame_height = height;
    this->frame_width = width;
}

void Framebuffer::fill(Framebuffer_color c)
{
}

void Framebuffer::pixel(uint8_t x, uint8_t y, Framebuffer_color c)
{
    assert(x >= 0 && x < this->frame_width && y >= 0 && y < this->frame_height);

    // The calculation to determine the correct bit to set depends on which address
    // mode we are in. This code assumes horizontal

    // The video ram on the SSD1306 is split up in to 8 rows, one bit per pixel.
    // Each row is 128 long by 8 pixels high, each byte vertically arranged, so byte 0 is x=0, y=0->7,
    // byte 1 is x = 1, y=0->7 etc

    // This code could be optimised, but is like this for clarity. The compiler
    // should do a half decent job optimising it anyway.

    const int BytesPerRow = this->frame_width; // x pixels, 1bpp, but each row is 8 pixel high, so (x / 8) * 8

    int byte_idx = (y / 8) * BytesPerRow + x;
    uint8_t byte = this->buffer[byte_idx];

    if (c==Framebuffer_color::white)
        byte |= 1 << (y % 8);
    else
        byte &= ~(1 << (y % 8));

    this->buffer[byte_idx] = byte;
}

void Framebuffer::hline(uint8_t x, uint8_t y, Framebuffer_color c)
{
}

void Framebuffer::vline(uint8_t x, uint8_t y, Framebuffer_color c)
{
}

void Framebuffer::line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, Framebuffer_color c)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true)
    {
        this->pixel( x0, y0, c);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;

        if (e2 >= dy)
        {
            err += dy;

            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void Framebuffer::rect(uint8_t x, uint8_t y, size_t w, size_t h, Framebuffer_color c, bool fill)
{
}

void Framebuffer::ellipse(uint8_t x, uint8_t y, uint8_t xr, uint8_t yr, Framebuffer_color c, bool fill, uint8_t m)
{
}

void Framebuffer::text(std::string s, uint8_t x, uint8_t y, Framebuffer_color c)
{
}
