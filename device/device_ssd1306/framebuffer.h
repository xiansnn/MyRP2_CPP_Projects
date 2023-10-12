#if !defined(FRAMEBUFFER_H)
#define FRAMEBUFFER_H

#include "pico/stdlib.h"
#include <string>

#include "5x8_font.h"
#include "8x8_font.h"
#include "12x16_font.h"
#include "16x32_font.h"

/// @brief
enum class Framebuffer_format
{
    MONO_VLSB,
    MONO_HLSB,
    MONO_HMSB,
    RGB565
};
/// @brief
enum class Framebuffer_color
{
    black = 0,
    white = 1
};

enum class WriteMode : const uint8_t
{
    ADD = 0,
    SUBTRACT = 1,
    INVERT = 2
};

enum class Rotation
{
    /// deg0 - means no rotation
    deg0,
    /// deg 90 - means 90 deg rotation
    deg90,
};

/// @brief
class Framebuffer
{
private:
    /* data */
    size_t frame_width;
    size_t frame_height;
    size_t buffer_size;
    Framebuffer_format format;

    static inline int get_font_index(uint8_t ch);
    void write_char(int16_t x, int16_t y, uint8_t ch);
    void write_string(int16_t x, int16_t y, const char *str);

    /// \brief Performs OR logical operation on selected and provided byte
    ///
    /// ex. if byte in buffer at position byte_idx is 0b10001111 and provided byte is 0b11110000 the buffer at position byte_idx becomes 0b11111111
    /// \param byte_idx - byte offset in buffer array to work on
    /// \param byte - provided byte to make operation
    void byteOR(int byte_idx, uint8_t byte);

    /// \brief Performs AND logical operation on selected and provided byte
    ///
    /// ex. if byte in buffer at position byte_idx is 0b10001111 and provided byte is 0b11110000 the buffer at position byte_idx becomes 0b10000000
    /// \param byte_idx - byte offset in buffer array to work on
    /// \param byte - provided byte to make operation
    void byteAND(int byte_idx, uint8_t byte);

    /// \brief Performs XOR logical operation on selected and provided byte
    ///
    /// ex. if byte in buffer at position byte_idx is 0b10001111 and provided byte is 0b11110000 the buffer at position byte_idx becomes 0b0111111
    /// \param byte_idx - byte offset in buffer array to work on
    /// \param byte - provided byte to make operation
    void byteXOR(int byte_idx, uint8_t byte);

public:
    /// @brief
    uint8_t *buffer;

    /// @brief
    /// @param buffer
    /// @param width
    /// @param height
    /// @param format
    Framebuffer(uint8_t buffer[], size_t width, size_t height, Framebuffer_format format);
    /// @brief
    /// @param c
    void fill(Framebuffer_color c);
    void clear_buffer();
    void pixel(uint8_t x, uint8_t y, Framebuffer_color c = Framebuffer_color::white);
    void hline(uint8_t x, uint8_t y, size_t w, Framebuffer_color c = Framebuffer_color::white);
    void vline(uint8_t x, uint8_t y, size_t h, Framebuffer_color c = Framebuffer_color::white);
    void line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, Framebuffer_color c = Framebuffer_color::white);
    void rect(uint8_t x, uint8_t y, size_t w, size_t h, bool fill = false, Framebuffer_color c = Framebuffer_color::white);
    void circle(int radius, int x_center, int y_center, bool fill = false, Framebuffer_color c = Framebuffer_color::white);
    void ellipse(uint8_t x_center, uint8_t y_center, uint8_t x_radius, uint8_t y_radius, bool fill = false, uint8_t quadrant = 0b1111, Framebuffer_color c = Framebuffer_color::white);
    void text(std::string str, uint16_t x, uint16_t y, Framebuffer_color c = Framebuffer_color::white);

    void setPixel(int16_t x, int16_t y, WriteMode mode);
    void drawChar(const unsigned char *font, char c, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode = WriteMode::ADD, Rotation = Rotation::deg0);
    void drawText(const unsigned char *font, char *text, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode = WriteMode::ADD, Rotation rotation = Rotation::deg0);
};

#endif // FRAMEBUFFER_H