#if !defined(FRAMEBUFFER_H)
#define FRAMEBUFFER_H

#include "pico/stdlib.h"
#include <string>

#include <cstdarg>

#define SSD1306_ASCII_FULL
#define FONT_WIDTH 0
#define FONT_HEIGHT 1
#include "5x8_font.h"
#include "8x8_font.h"
#include "12x16_font.h"
#include "16x32_font.h"
#define BACKSPACE '\b'
#define HORIZONTAL_TAB '\t'
#define LINE_FEED '\n'
#define VERTICAL_TAB '\v'
#define FORM_FEED '\f'
#define CARRIAGE_RETURN '\r'

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

typedef struct struct_frame_data
{
    int anchor_x{0};
    int anchor_y{0};
    int width{0};
    int height{0};
} frame_data_t;

typedef struct struct_text_frame
{
    const unsigned char *font{nullptr};
    uint8_t tab_size {2};
    Framebuffer_color fg_color{Framebuffer_color::white};
    Framebuffer_color bg_color{Framebuffer_color::black};

} text_config_t;

class Framebuffer
{
private:
    size_t buffer_size;
    Framebuffer_format format;
    text_config_t text_config{};
    uint8_t current_char_line{0};
    uint8_t current_char_column{0};

    void ellipse(uint8_t x_center, uint8_t y_center, uint8_t x_radius, uint8_t y_radius, bool fill, uint8_t quadrant, Framebuffer_color c);
    void drawChar(const unsigned char *font, char c, uint8_t anchor_x, uint8_t anchor_y);
    void drawChar( char c, uint8_t char_column, uint8_t char_line);
    void pixel(int x, int y, Framebuffer_color c = Framebuffer_color::white);
    void next_line();
    void next_char();
    void clear_line();

public:
    uint8_t *buffer;
    uint8_t frame_width;
    uint8_t frame_height;
    uint8_t max_line{0};
    uint8_t max_column{0};

    Framebuffer(size_t width, size_t height, Framebuffer_format format);
    ~Framebuffer();

    void fill(Framebuffer_color c);
    void clear_buffer();
    void hline(uint8_t x, uint8_t y, size_t w, Framebuffer_color c = Framebuffer_color::white);
    void vline(uint8_t x, uint8_t y, size_t h, Framebuffer_color c = Framebuffer_color::white);
    void line(int x1, int y1, int x2, int y2, Framebuffer_color c = Framebuffer_color::white);
    void rect(uint8_t x, uint8_t y, size_t w, size_t h, bool fill = false, Framebuffer_color c = Framebuffer_color::white);
    void circle(int radius, int x_center, int y_center, bool fill = false, Framebuffer_color c = Framebuffer_color::white);

    void text(const unsigned char *font, std::string text, uint8_t anchor_x, uint8_t anchor_y, Framebuffer_color c = Framebuffer_color::white);
    void text(const unsigned char *font, char *c_str, uint8_t anchor_x, uint8_t anchor_y, Framebuffer_color c = Framebuffer_color::white);
    void set_text_config(text_config_t device_config);
    void set_font( const unsigned char * font);
    void print_text(const char *c_str);


};

#endif // FRAMEBUFFER_H