#if !defined(SSD1306_H)
#define SSD1306_H

#include "commands_SSD1306.h"
#include "pico/stdlib.h"
#include "hw_i2c.h"
#include "framebuffer.h"

// Time_frame_interval
#define _2_FRAMES 0b111
#define _3_FRAMES 0b100
#define _4_FRAMES 0b101
#define _5_FRAMES 0b000
#define _25_FRAMES 0b110
#define _64_FRAMES 0b001
#define _128_FRAMES 0b010
#define _256_FRAMES 0b011

// addressing mode
#define HORIZONTAL_ADDRESSING_MODE 0
#define VERTICAL_ADDRESSING_MODE 1
#define PAGE_ADDRESSING_MODE 2

typedef struct init_config_SSD1306

{
    uint8_t i2c_address = 0x3C;
    uint8_t mux_ratio_value = 64;
    uint8_t vertical_offset = 0;
    uint8_t GDDRAM_start_line = 0;
    bool scan_SEG_inverse_direction = false;
    bool scan_COM_inverse_direction = false;
    bool sequential_COM = false;
    bool enable_COM_L_R_remap = false;
    uint8_t contrast = 127;
    uint8_t frequency_divider = 1;
    uint8_t frequency_factor = 0;
} init_config_SSD1306_t;

typedef struct config_scroll_ssd1306
{
    bool scroll_H_to_right = true;           // if true SSD1306_SET_R_HORIZ_SCROLL else SSD1306_SET_L_HORIZ_SCROLL
    bool scroll_V_and_H_to_right = true;     // if true SSD1306_SET_VERTICAL_R_HORIZ_SCROLL else SSD1306_SET_VERTICAL_L_HORIZ_SCROLL
    uint8_t scroll_H_start_page = 0;         // 0 <= value <= 7
    uint8_t time_frame_interval = _2_FRAMES; // 0 <= value <= 7
    uint8_t scroll_H_end_page = 7;           // 0 <= value <= 7
    uint8_t vertical_scrolling_offset = 5;   // 0 <= value <= 63
} config_scroll_t;

typedef struct struct_render_area
{
    uint8_t start_col{0};
    uint8_t end_col{SSD1306_WIDTH - 1};
    uint8_t start_page{0};
    uint8_t end_page{SSD1306_NUM_PAGES - 1};
    size_t width{SSD1306_WIDTH};
    size_t height{SSD1306_HEIGHT};
    size_t buflen{SSD1306_BUF_LEN};
} render_area_t;

class SSD1306 : public Framebuffer
{
private:
    hw_I2C_master *i2c_master;
    init_config_SSD1306_t config;

    void init();
    void init_MUX_ratio(uint8_t value);
    void init_display_vertical_shift(uint8_t value);
    void init_RAM_start_line(uint8_t value);
    void init_SEG_scan_inverse_direction(bool inverse);
    void init_COM_scan_inverse_direction(bool inverse);
    void init_COM_cfg(bool sequentialCOM, bool enableCOMLRremap);
    void init_clock_frequency(uint8_t divide_ratio, uint8_t frequency_factor);
    void init_charge_pump_enabled(bool enabled);
    void send_cmd(uint8_t cmd);
    void send_cmd_list(uint8_t *cmd_list, int num);
    void send_buf(uint8_t buf[], size_t buflen);

public:
    SSD1306(hw_I2C_master *master, init_config_SSD1306_t config);
    void set_contrast(uint8_t value);
    static render_area_t compute_render_area(uint8_t start_col, uint8_t end_col, uint8_t start_line, uint8_t end_line);
    void show_render_area(uint8_t *data_buffer, render_area_t screen_area, uint8_t addressing_mode = HORIZONTAL_ADDRESSING_MODE);
    void show();
    void show(Framebuffer *frame, uint8_t anchor_x, uint8_t anchor_y);
    void show(Framebuffer *frame, frame_data_t data);
    void fill_pattern_and_show_GDDRAM(uint8_t pattern, render_area_t area);
    void clear_buffer_and_show_GDDRAM();

    void set_display_from_RAM();
    void set_all_pixel_ON();
    void set_inverse_color(bool inverse);
    void set_display_OFF();
    void set_display_ON();
    void horizontal_scroll(bool on, config_scroll_t scroll_data);
    void vertical_scroll(bool on, config_scroll_t scroll_data);
};

#endif // SSD1306_H
