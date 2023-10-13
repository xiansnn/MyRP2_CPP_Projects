#if !defined(FRAMEBUFFER_H)
#define FRAMEBUFFER_H

#include "pico/stdlib.h"
#include <string>



#define  SSD1306_ASCII_FULL
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

/// @brief
class Framebuffer
{
private:
    uint8_t frame_width;
    uint8_t frame_height;
    uint16_t buffer_size;
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

    void drawChar(const unsigned char *font, char c, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode = WriteMode::ADD);
    void pixel(uint8_t x, uint8_t y, Framebuffer_color c = Framebuffer_color::white);

public:
    /// @brief
    uint8_t *buffer;

    /// @brief
    /// @param width
    /// @param height
    /// @param format
    Framebuffer(size_t width, size_t height, Framebuffer_format format);
    ~Framebuffer();
    /// @brief
    /// @param c
    void fill(Framebuffer_color c);
    void clear_buffer();
    void hline(uint8_t x, uint8_t y, size_t w, Framebuffer_color c = Framebuffer_color::white);
    void vline(uint8_t x, uint8_t y, size_t h, Framebuffer_color c = Framebuffer_color::white);
    void line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, Framebuffer_color c = Framebuffer_color::white);
    void rect(uint8_t x, uint8_t y, size_t w, size_t h, bool fill = false, Framebuffer_color c = Framebuffer_color::white);
    void circle(int radius, int x_center, int y_center, bool fill = false, Framebuffer_color c = Framebuffer_color::white);
    void text(const unsigned char *font, std::string text, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode = WriteMode::ADD);
    };

#endif // FRAMEBUFFER_H