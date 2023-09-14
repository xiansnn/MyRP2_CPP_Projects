#include "framebuffer.h"

Framebuffer::Framebuffer()
{
}

Framebuffer::Framebuffer(uint8_t buffer[], size_t width, size_t height, Framebuffer_format format)
{
    assert( format == Framebuffer_format::MONO_VLSB);
}

void Framebuffer::fill(Framebuffer_color c)
{
}

void Framebuffer::pixel(uint8_t x, uint8_t y, Framebuffer_color c)
{
}

void Framebuffer::hline(uint8_t x, uint8_t y, Framebuffer_color c)
{
}

void Framebuffer::vline(uint8_t x, uint8_t y, Framebuffer_color c)
{
}

void Framebuffer::line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, Framebuffer_color c)
{
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
