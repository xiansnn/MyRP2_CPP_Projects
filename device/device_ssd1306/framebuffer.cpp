#include "framebuffer.h"
#include <string.h>
#include <cstring>

#define BYTE_SIZE 8

Framebuffer::Framebuffer(size_t width, size_t height, Framebuffer_format format)
{
    assert(format == Framebuffer_format::MONO_VLSB); // works only for MONO_VLSB devices
    this->format = format;
    this->frame_height = height;
    this->frame_width = width; // MONO_VLDB => 1 Byte = 1 column of 8 pixel
    size_t page_nb = height / BYTE_SIZE;
    if (height % BYTE_SIZE != 0)
        page_nb += 1;
    this->buffer_size = width * page_nb;
    this->buffer = new uint8_t[this->buffer_size];
    clear_buffer();
}

Framebuffer::~Framebuffer()
{
    delete[] this->buffer;
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
    current_char_column = 0;
    current_char_line = 0;
}

void Framebuffer::pixel(int x, int y, Framebuffer_color c)
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

void Framebuffer::line(int x0, int y0, int x1, int y1, Framebuffer_color c)
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

// void Framebuffer::byteOR(int byte_idx, uint8_t byte)
// {
//     // return if index outside 0 - buffer length - 1
//     if (byte_idx > (this->buffer_size - 1))
//         return;
//     this->buffer[byte_idx] |= byte;
// }
// void Framebuffer::byteAND(int byte_idx, uint8_t byte)
// {
//     // return if index outside 0 - buffer length - 1
//     if (byte_idx > (this->buffer_size - 1))
//         return;
//     this->buffer[byte_idx] &= byte;
// }
// void Framebuffer::byteXOR(int byte_idx, uint8_t byte)
// {
//     // return if index outside 0 - buffer length - 1
//     if (byte_idx > (this->buffer_size - 1))
//         return;
//     this->buffer[byte_idx] ^= byte;
// }

void Framebuffer::drawChar(const unsigned char *font, char c, uint8_t anchor_x, uint8_t anchor_y)
{
    if (!font || c < 32)
        return;

    uint8_t font_width = font[FONT_WIDTH];
    uint8_t font_height = font[FONT_HEIGHT];

    uint16_t seek = (c - 32) * (font_width * font_height) / 8 + 2;

    uint8_t b_seek = 0;

    for (uint8_t x = 0; x < font_width; x++)
    {
        for (uint8_t y = 0; y < font_height; y++)
        {
            if (font[seek] >> b_seek & 0b00000001)
                this->pixel(x + anchor_x, y + anchor_y, this->text_config.fg_color);
            else
                this->pixel(x + anchor_x, y + anchor_y, this->text_config.bg_color);

            b_seek++;
            if (b_seek == 8)
            {
                b_seek = 0;
                seek++;
            }
        }
    }
}

void Framebuffer::drawChar(char c, uint8_t char_column, uint8_t char_line)
{
    uint8_t anchor_x = char_column * text_config.font[FONT_WIDTH];
    uint8_t anchor_y = char_line * text_config.font[FONT_HEIGHT];
    drawChar(this->text_config.font, c, anchor_x, anchor_y);
}

void Framebuffer::text(const unsigned char *font, std::string text, uint8_t anchor_x, uint8_t anchor_y, Framebuffer_color color)
{
    char *c_str = new char[text.length() + 1];
    std::strcpy(c_str, text.c_str());

    this->text(font, c_str, anchor_x, anchor_y, color);

    delete[] c_str;
}

void Framebuffer::text(const unsigned char *font, char *c_str, uint8_t anchor_x, uint8_t anchor_y, Framebuffer_color color)
{
    uint8_t font_width = font[FONT_WIDTH];
    uint16_t n = 0;
    while (c_str[n] != '\0')
    {
        drawChar(font, c_str[n], anchor_x + (n * font_width), anchor_y);
        n++;
    }
}

void Framebuffer::set_text_config(text_config_t device_config)
{
    this->text_config = device_config;
    this->max_column = this->frame_width / device_config.font[FONT_WIDTH];
    this->max_line = this->frame_height / device_config.font[FONT_HEIGHT];
}

void Framebuffer::set_font(const unsigned char *font)
{
    this->text_config.font = font;
    this->max_column = this->frame_width / text_config.font[FONT_WIDTH];
    this->max_line = this->frame_height / text_config.font[FONT_HEIGHT];
}

void Framebuffer::next_line()
{
    current_char_line++;
    current_char_column = 0;
    if (current_char_line >= max_line)
        current_char_line = 0;
}

void Framebuffer::next_char()
{
    current_char_column++;
    if (current_char_column >= max_column)
    {
        current_char_column = 0;
        next_line();
    }
}

void Framebuffer::clear_line()
{ // TODO ecrire 0x00 dans le buffer sur la largeur de frame et sur anchor_y
    for (uint8_t i = 0; i < this->max_column; i++)
    {
        drawChar(' ', i, current_char_line);
    }
}

void Framebuffer::print_text(const char *c_str)
{
    uint16_t n = 0;
    while (c_str[n] != '\0')
    {
        switch (c_str[n])
        {
        case LINE_FEED:
            next_line();
            current_char_column = 0;
            break;
        // case HORIZONTAL_TAB:
        //     for (uint8_t i = 0; i < text_config.tab_size; i++)
        //     {
        //         drawChar(' ', current_char_column, current_char_line);
        //         next_char();
        //     }
        //     break;
        case BACKSPACE: // TODO
            current_char_column--;
            drawChar(' ', current_char_column, current_char_line);
            break;
        case FORM_FEED: // TODO
            clear_buffer();
            current_char_column = 0;
            current_char_line = 0;
            break;
        case CARRIAGE_RETURN: // TODO
            current_char_column = 0;
            break;
        default:
            if (current_char_column == 0)
                clear_line(); // start a new line
            if (c_str[n] == HORIZONTAL_TAB)
            {
                for (uint8_t i = 0; i < text_config.tab_size; i++)
                {
                    drawChar(' ', current_char_column, current_char_line);
                    next_char();
                }
            }
            else
            {
                drawChar(c_str[n], current_char_column, current_char_line);
                next_char();
            }
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
