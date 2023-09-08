#if !defined(SSD1306_H)
#define SSD1306_H

#include "commands_SSD1306.h"
#include "pico/stdlib.h"
#include "hw_I2C/hw_i2c.h"


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
    uint8_t frequency_factor = 8;
} init_config_SSD1306_t;

// typedef struct scroll_config_ssd1306
// {
//     bool scroll_H_to_right = true;       // if true SSD1306_SET_R_HORIZ_SCROLL else SSD1306_SET_L_HORIZ_SCROLL
//     bool scroll_V_and_H_to_right = true; // if true SSD1306_SET_VERTICAL_R_HORIZ_SCROLL else SSD1306_SET_VERTICAL_L_HORIZ_SCROLL
//     uint8_t scroll_H_start_page = 0;     // 0 <= value <= 7
//     uint8_t time_frame_interval = _5_FRAMES;
//     uint8_t scroll_H_end_page = 7; // 0 <= value <= 7
// } scroll_config_ssd1306_t;

struct render_area
{
    uint8_t start_col{0};
    uint8_t end_col{127};
    uint8_t start_page{0};
    uint8_t end_page{7};
    int buflen{SSD1306_BUF_LEN};
};

class SSD1306
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
    void init_contrast(uint8_t value);
    void init_clock_frequency ( uint8_t divide_ratio, uint8_t frequency_factor);
    void init_charge_pump_enabled(bool enabled);

public:
    SSD1306(hw_I2C_master *master, init_config_SSD1306_t config);

    void send_cmd(uint8_t cmd);
    void send_cmd_list(uint8_t *cmd_list, int num);
    void send_buf(uint8_t buf[], size_t buflen);
    void set_display_from_RAM();
    void set_all_pixel_ON();
    void set_inverse_color(bool inverse);
    void set_display_OFF();
    void set_display_ON();

    struct render_area area;
    void calc_render_area_buflen();
    uint8_t GDDRAM_buffer[SSD1306_BUF_LEN];
    void scroll(bool on);
    void render(uint8_t *buf);
    static void SetPixel(uint8_t *buf, int x,int y, bool on);
    static void DrawLine(uint8_t *buf, int x0, int y0, int x1, int y1, bool on);
    static inline int GetFontIndex(uint8_t ch);
    static void WriteChar(uint8_t *buf, int16_t x, int16_t y, uint8_t ch);
    static void WriteString(uint8_t *buf, int16_t x, int16_t y, char *str);
    
};

#endif // SSD1306_H
    