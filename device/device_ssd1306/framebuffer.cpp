#include "framebuffer.h"
#include <string.h>
#include <cstring>
#include "ssd1306_font.h"

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
}

void Framebuffer::fill(Framebuffer_color c)
{
    assert(this->format == Framebuffer_format::MONO_VLSB);
    // uint8_t image[SSD1306_BUF_LEN];
    int pattern;
    if (c == Framebuffer_color::black)
        pattern = 0x00;
    else
        pattern = 0xFF;
    memset(this->buffer, pattern, this->buffer_size);
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

void Framebuffer::setPixel(int16_t x, int16_t y, WriteMode mode)
{
    // return if position out of bounds
    if ((x < 0) || (x >= this->frame_width) || (y < 0) || (y >= this->frame_height))
        return;

    // byte to be used for buffer operation
    uint8_t byte;

    byte = 1 << (y & 7);
    // display with 32 px height requires doubling of set bits, reason to this is explained in readme
    // this shifts 1 to byte based on y coordinate
    // remember that buffer is a one dimension array, so we have to calculate offset from coordinates
    // if (size == Size::W128xH32)
    // {
    //     y = (y << 1) + 1;
    //     byte = 1 << (y & 7);
    //     char byte_offset = byte >> 1;
    //     byte = byte | byte_offset;
    // }
    // else
    // {
    // }

    // check the write mode and manipulate the frame buffer
    if (mode == WriteMode::ADD)
    {
        this->byteOR(x + (y / 8) * this->frame_width, byte);
    }
    else if (mode == WriteMode::SUBTRACT)
    {
        this->byteAND(x + (y / 8) * this->frame_width, ~byte);
    }
    else if (mode == WriteMode::INVERT)
    {
        this->byteXOR(x + (y / 8) * this->frame_width, byte);
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

/// @brief doesn't work
/// @param x_center
/// @param y_center
/// @param x_radius
/// @param y_radius
/// @param fill
/// @param quadrant
/// @param c
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

inline int Framebuffer::get_font_index(uint8_t ch)
{
    if (ch >= 'A' && ch <= 'Z')
    {
        return ch - 'A' + 1;
    }
    else if (ch >= '0' && ch <= '9')
    {
        return ch - '0' + 27;
    }
    else
        return 0; // Not got that char so space.
}
unsigned char reverseBits(unsigned char c)
{
    unsigned char result = 0;
    for (int shift = 0; shift < BYTE_SIZE; shift++)
    {
        if (c & (0x01 << shift))
        {
            result |= (0x80 >> shift);
        }
    }
    return result;
}

void Framebuffer::write_char(int16_t x, int16_t y, uint8_t ch)
{
    if (x > this->frame_width - 8 || y > this->frame_height - 8)
        return;
    y = y / 8; // For the moment, only write on Y row boundaries (every 8 vertical pixels)

    ch = toupper(ch);
    int idx = get_font_index(ch);
    int fb_idx = y * 128 + x;

    for (int i = 0; i < 8; i++)
    {
        // buf[fb_idx++] = reversed[idx * 8 + i];
        this->buffer[fb_idx++] = reverseBits(font[idx * 8 + i]);
    }
}

void Framebuffer::write_string(int16_t x, int16_t y, const char *txt)
{
    // Cull out any string off the screen
    if (x > this->frame_width - 8 || y > this->frame_height - 8)
        return;

    while (*txt)
    {
        this->write_char(x, y, *txt++);
        x += 8;
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

void Framebuffer::text(std::string str, uint16_t x, uint16_t y, Framebuffer_color c)
{
    const char *txt = str.c_str();
    this->write_string(x, y, txt);
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
                    // this->setPixel(x + anchor_x, y + anchor_y, mode);
                    this->pixel(x + anchor_x, y + anchor_y);
                    break;
                case Rotation::deg90:
                    // this->setPixel(-y + anchor_x + font_height, x + anchor_y, mode);
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

void Framebuffer::drawText(const unsigned char *font, char *text, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode , Rotation rotation)
{
    if (!font || !text)
        return;

    uint8_t font_width = font[0];

    uint16_t n = 0;
    while (text[n] != '\0')
    {
        switch (rotation)
        {
        case Rotation::deg0:
            drawChar( font, text[n], anchor_x + (n * font_width), anchor_y, mode, rotation);
            break;
        case Rotation::deg90:
            drawChar( font, text[n], anchor_x, anchor_y + (n * font_width), mode, rotation);
            break;
        }

        n++;
    }
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
