#include "framebuffer.h"
#include <string.h>
#include <cstring>

#define BYTE_SIZE 8

Framebuffer::Framebuffer(uint8_t buffer[], size_t width, size_t height, Framebuffer_format format)
{
    assert(format == Framebuffer_format::MONO_VLSB); // works only for MONO_VLSB devices
    this->format = format;
    this->buffer = buffer;
    this->frame_height = height;
    this->frame_width = width; // MONO_VLDB => 1 Byte = 1 column of 8 pixel
    size_t page_nb = height / BYTE_SIZE;
    if (height % BYTE_SIZE != 0)
        page_nb += 1;
    this->buffer_size = width * page_nb;
    clear_buffer();
}

void Framebuffer::fill(Framebuffer_color c)
{
    assert(this->format == Framebuffer_format::MONO_VLSB);
    int pattern;
    if (c == Framebuffer_color::black)
        pattern = 0x00;
    else
        pattern = 0xFF;
    memset(this->buffer, pattern, this->buffer_size);
}

void Framebuffer::clear_buffer()
{
    assert(this->format == Framebuffer_format::MONO_VLSB);
    memset(this->buffer, 0x00, this->buffer_size);
}

void Framebuffer::pixel(uint8_t x, uint8_t y, Framebuffer_color c)
{
    assert(format == Framebuffer_format::MONO_VLSB); // works only if MONO_VLSB

    if (x >= 0 && x < this->frame_width && y >= 0 && y < this->frame_height) // avoid drawing outside the framebuffer
    {
        const int BytesPerRow = this->frame_width; // x pixels, 1bpp, but each row is 8 pixel high, so (x / 8) * 8
        int byte_idx = (y / 8) * BytesPerRow + x;
        uint8_t byte = this->buffer[byte_idx];

        if (c == Framebuffer_color::white)
            byte |= 1 << (y % 8);
        else
            byte &= ~(1 << (y % 8));

        this->buffer[byte_idx] = byte;
    }
}

void Framebuffer::hline(uint8_t x, uint8_t y, size_t w, Framebuffer_color c)
{
    for (size_t i = 0; i < w; i++)
        this->pixel(x + i, y, c);
}

void Framebuffer::vline(uint8_t x, uint8_t y, size_t h, Framebuffer_color c)
{
    for (size_t i = 0; i < h; i++)
        this->pixel(x, y + i, c);
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
        this->pixel(x0, y0, c);
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

void Framebuffer::rect(uint8_t x, uint8_t y, size_t w, size_t h, bool fill, Framebuffer_color c)
{
    if (!fill)
    {
        hline(x, y, w, c);
        hline(x, y + h - 1, w, c);
        vline(x, y, h, c);
        vline(x + w - 1, y, h, c);
    }
    else
    {
        for (size_t i = 0; i < h; i++)
            hline(x, y + i, w, c);
    }
}


void Framebuffer::ellipse(uint8_t x_center, uint8_t y_center, uint8_t x_radius, uint8_t y_radius, bool fill, uint8_t quadrant, Framebuffer_color c)
{
    int x, y, m;
    x = 0;
    y = y_radius;
    int _xr2 = x_radius * x_radius;
    int _yr2 = y_radius * y_radius;

    m = 4 * _yr2 - 4 * _xr2 * y_radius + _xr2;

    while (y >= 0)
    {
        if (!fill)
        {
            pixel(x_center + x, y_center + y, c);
            pixel(x_center - x, y_center + y, c);
            pixel(x_center + x, y_center - y, c);
            pixel(x_center - x, y_center - y, c);
        }
        else
        {
            hline(x_center - x, y_center + y, 2 * x + 2, c);
            hline(x_center - y, y_center + x, 2 * y + 2, c);
            hline(x_center - y, y_center - x, 2 * y + 2, c);
            hline(x_center - x, y_center - y, 2 * x + 2, c);
        }

        if (m > 0)
        {
            x += 1;
            y -= 1;
            m += 3 * _xr2 - 4 * _xr2 * y;
        }
        else
        {
            x += 1;
            m += 4 * _xr2 * y - _xr2;
        }
    }
}

void Framebuffer::byteOR(int byte_idx, uint8_t byte)
{
    // return if index outside 0 - buffer length - 1
    if (byte_idx > (this->buffer_size - 1))
        return;
    this->buffer[byte_idx] |= byte;
}
void Framebuffer::byteAND(int byte_idx, uint8_t byte)
{
    // return if index outside 0 - buffer length - 1
    if (byte_idx > (this->buffer_size - 1))
        return;
    this->buffer[byte_idx] &= byte;
}
void Framebuffer::byteXOR(int byte_idx, uint8_t byte)
{
    // return if index outside 0 - buffer length - 1
    if (byte_idx > (this->buffer_size - 1))
        return;
    this->buffer[byte_idx] ^= byte;
}


void Framebuffer::drawChar(const unsigned char *font, char c, uint8_t anchor_x, uint8_t anchor_y,
                           WriteMode mode, Rotation rotation)
{
    if (!font || c < 32)
        return;

    uint8_t font_width = font[0];
    uint8_t font_height = font[1];

    uint16_t seek = (c - 32) * (font_width * font_height) / 8 + 2;

    uint8_t b_seek = 0;

    for (uint8_t x = 0; x < font_width; x++)
    {
        for (uint8_t y = 0; y < font_height; y++)
        {
            if (font[seek] >> b_seek & 0b00000001)
            {
                switch (rotation)
                {
                case Rotation::deg0:
                    this->pixel(x + anchor_x, y + anchor_y);
                    break;
                case Rotation::deg90:
                    this->pixel(-y + anchor_x + font_height, x + anchor_y);
                    break;
                }
            }
            b_seek++;
            if (b_seek == 8)
            {
                b_seek = 0;
                seek++;
            }
        }
    }
}

void Framebuffer::text(const unsigned char *font, std::string text, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode, Rotation rotation)
{
    uint8_t font_width = font[0];

    char *cstr = new char[text.length() + 1];
    std::strcpy(cstr, text.c_str());

    uint16_t n = 0;
    while (cstr[n] != '\0')
    {
        switch (rotation)
        {
        case Rotation::deg0:
            drawChar(font, cstr[n], anchor_x + (n * font_width), anchor_y, mode, rotation);
            break;
        case Rotation::deg90:
            drawChar(font, cstr[n], anchor_x, anchor_y + (n * font_width), mode, rotation);
            break;
        }

        n++;
    }
    delete[] cstr;
}

void Framebuffer::circle(int radius, int x_center, int y_center, bool fill, Framebuffer_color c)
/*
https://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_d%27arc_de_cercle_de_Bresenham
https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
procédure tracerCercle (entier rayon, entier x_centre, entier y_centre)
    déclarer entier x, y, m ;
    x ← 0 ;
    y ← rayon ;             // on se place en haut du cercle
    m ← 5 - 4*rayon ;       // initialisation
    Tant que x <= y         // tant qu'on est dans le second octant
        tracerPixel( x+x_centre, y+y_centre ) ;
        tracerPixel( y+x_centre, x+y_centre ) ;
        tracerPixel( -x+x_centre, y+y_centre ) ;
        tracerPixel( -y+x_centre, x+y_centre ) ;
        tracerPixel( x+x_centre, -y+y_centre ) ;
        tracerPixel( y+x_centre, -x+y_centre ) ;
        tracerPixel( -x+x_centre, -y+y_centre ) ;
        tracerPixel( -y+x_centre, -x+y_centre ) ;
        si m > 0 alors	//choix du point F
            y ← y - 1 ;
            m ← m - 8*y ;
        fin si ;
        x ← x + 1 ;
        m ← m + 8*x + 4 ;
    fin tant que ;
fin de procédure ;
*/
{
    int x, y, m;
    x = 0;
    y = radius;
    m = 5 - 4 * radius;
    while (x <= y)
    {
        if (!fill)
        {
            pixel(x_center + x, y_center + y, c);
            pixel(x_center + y, y_center + x, c);
            pixel(x_center - x, y_center + y, c);
            pixel(x_center - y, y_center + x, c);
            pixel(x_center + x, y_center - y, c);
            pixel(x_center + y, y_center - x, c);
            pixel(x_center - x, y_center - y, c);
            pixel(x_center - y, y_center - x, c);
        }
        else
        {
            hline(x_center - x, y_center + y, 2 * x + 2, c);
            hline(x_center - y, y_center + x, 2 * y + 2, c);
            hline(x_center - y, y_center - x, 2 * y + 2, c);
            hline(x_center - x, y_center - y, 2 * x + 2, c);
        }
        if (m > 0)
        {
            y -= 1;
            m -= 8 * y;
        }
        x += 1;
        m += 8 * x + 4;
    }
}
