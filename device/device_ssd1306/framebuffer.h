#if !defined(FRAMEBUFFER_H)
#define FRAMEBUFFER_H

#include "pico/stdlib.h"
#include <string>

#define SSD1306_ASCII_FULL
#define FONT_WIDTH 0
#define FONT_HEIGHT 1
#include "5x8_font.h"
#include "8x8_font.h"
#include "12x16_font.h"
#include "16x32_font.h"

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

class Framebuffer
{
private:
    size_t buffer_size;
    Framebuffer_format format;


    /// @brief dosn't work! waiting to be fixed
    /// @param x_center
    /// @param y_center
    /// @param x_radius
    /// @param y_radius
    /// @param fill
    /// @param quadrant
    /// @param c
    void ellipse(uint8_t x_center, uint8_t y_center, uint8_t x_radius, uint8_t y_radius, bool fill, uint8_t quadrant, Framebuffer_color c);

    void drawChar(const unsigned char *font, char c, uint8_t anchor_x, uint8_t anchor_y, Framebuffer_color color = Framebuffer_color::white);
    void pixel(int x, int y, Framebuffer_color c = Framebuffer_color::white);

public:
    uint8_t *buffer;
    uint8_t frame_width;
    uint8_t frame_height;

    Framebuffer(size_t width, size_t height, Framebuffer_format format);
    // Framebuffer(frame_data_t data, Framebuffer_format format);
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
};

#endif // FRAMEBUFFER_H