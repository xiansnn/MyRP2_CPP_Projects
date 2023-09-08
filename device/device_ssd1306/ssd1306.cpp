#include "ssd1306.h"

void SSD1306::send_cmd(uint8_t cmd)
{
    // I2C write process expects a control byte followed by data
    // this "data" can be a command or data to follow up a command
    this->i2c_master->single_byte_write(this->config.i2c_address, I2C_CMD_FLAG, cmd);
}

void SSD1306::send_cmd_list(uint8_t *cmd_list, int cmd_list_size)
{
    for (int i = 0; i < cmd_list_size; i++)
        send_cmd(cmd_list[i]);
}

void SSD1306::send_buf(uint8_t buffer[], size_t buffer_size)
{
    this->i2c_master->burst_byte_write(this->config.i2c_address, I2C_DATA_FLAG, buffer, buffer_size);
}

void SSD1306::calc_render_area_buflen()
{ // calculate how long the flattened buffer will be for a render area
    this->area.buflen = (this->area.end_col - this->area.start_col + 1) * (this->area.end_page - this->area.start_page + 1);
}

SSD1306::SSD1306(hw_I2C_master *master, init_config_SSD1306_t init_config)
{
    this->i2c_master = master;
    this->config = init_config;
    this->init();
}

void SSD1306::set_display_from_RAM()
{
    send_cmd(SSD1306_SET_RAM_DISPLAY);
}

void SSD1306::set_all_pixel_ON()
{
    send_cmd(SSD1306_SET_DISPLAY_ON);
}

void SSD1306::set_inverse_color(bool inverse)
{
    if (inverse)
        send_cmd(SSD1306_SET_INV_DISP);
    else
        send_cmd(SSD1306_SET_NORM_DISP);
}

void SSD1306::set_display_OFF()
{
    send_cmd(SSD1306_SET_DISPLAY_SLEEP);
}

void SSD1306::set_display_ON()
{
    send_cmd(SSD1306_SET_DISPLAY_NORMAL_ON);
}

void SSD1306::init()
{
    printf("init_display\n");
    set_display_OFF();
    init_MUX_ratio(config.mux_ratio_value);
    init_display_vertical_shift(config.vertical_offset);
    init_RAM_start_line(config.GDDRAM_start_line);
    init_SEG_scan_inverse_direction(config.scan_SEG_inverse_direction);
    init_COM_scan_inverse_direction(config.scan_COM_inverse_direction);
    init_COM_cfg(config.sequential_COM, config.enable_COM_L_R_remap);
    init_contrast(config.contrast);
    set_display_from_RAM();
    set_inverse_color(false);
    init_clock_frequency(config.frequency_divider, config.frequency_factor);
    init_charge_pump_enabled(true);
    set_display_ON();

    uint8_t cmds[] = {
        SSD1306_SET_MEM_MODE,   // set memory address mode 0 = horizontal, 1 = vertical, 2 = page
        0x00,                   // horizontal addressing mode
        SSD1306_SET_SCROLL_OFF, // deactivate horizontal scrolling if set. This is necessary as memory writes will corrupt if scrolling was enabled
    };

    send_cmd_list(cmds, count_of(cmds));
}

void SSD1306::init_MUX_ratio(uint8_t value)
{
    assert((value >= 1) & (value <= 64));
    send_cmd(SSD1306_SET_MUX_RATIO);
    send_cmd(value - 1);
}

void SSD1306::init_display_vertical_shift(uint8_t value)
{
    assert((value >= 0) & (value <= 63));
    send_cmd(SSD1306_SET_DISP_OFFSET);
    send_cmd(value);
}

void SSD1306::init_RAM_start_line(uint8_t value)
{
    assert((value >= 0) & (value <= 63));
    send_cmd(SSD1306_SET_DISP_START_LINE);
    send_cmd(value);
}

void SSD1306::init_SEG_scan_inverse_direction(bool inverse)
{
    if (inverse == true)
        send_cmd(SSD1306_SET_SEG_REMAP);
    else
        send_cmd(SSD1306_SET_SEG_NO_REMAP);
}

void SSD1306::init_COM_scan_inverse_direction(bool inverse)
{
    if (inverse == true)
        send_cmd(SSD1306_SET_COM_OUT_DIR_REMAP);
    else
        send_cmd(SSD1306_SET_COM_OUT_DIR);
}

void SSD1306::init_COM_cfg(bool sequential_COM, bool enable_COM_L_R_remap)
{
    uint8_t value = 0x02;
    send_cmd(SSD1306_SET_COM_PIN_CFG);
    if (!sequential_COM)
    {
        value = value | 0x10;
    }
    if (enable_COM_L_R_remap)
    {
        value = value | 0x20;
    }
    send_cmd(value);
}

void SSD1306::init_contrast(uint8_t value)
{
    assert((value >= 0) & (value <= 255));
    send_cmd(SSD1306_SET_CONTRAST);
    send_cmd(value);
}

void SSD1306::init_clock_frequency(uint8_t divide_ratio, uint8_t frequency_factor)
{
    assert((divide_ratio >= 1) & (divide_ratio <= 16));
    assert((frequency_factor >= 0) & (frequency_factor <= 15));
    uint8_t value = (frequency_factor << 4) | (divide_ratio - 1);
    send_cmd(SSD1306_SET_DISP_CLK_DIV);
    send_cmd(value);
}

void SSD1306::init_charge_pump_enabled(bool enabled)
{
    send_cmd(SSD1306_SET_PRECHARGE);
    send_cmd(0x22); // 0xF1
    send_cmd(SSD1306_SET_VCOM_DESEL);
    send_cmd(0x20); // 0x30
    uint8_t value;
    send_cmd(SSD1306_SET_CHARGE_PUMP);
    if (enabled)
        value = 0x14;
    else
        value = 0x10;
    send_cmd(value);
}

void SSD1306::scroll(bool on)
{ // configure horizontal scrolling
    uint8_t cmds[] = {
        SSD1306_SET_R_HORIZ_SCROLL,
        0x00, // dummy byte
        0x01, // start page 0
        0x00, // time interval
        0x03, // end page 3 SSD1306_NUM_PAGES ??
        0x00, // dummy byte
        0xFF, // dummy byte
        (on ? SSD1306_SET_SCROLL_ON : SSD1306_SET_SCROLL_OFF)};
    this->send_cmd_list(cmds, count_of(cmds));
}

void SSD1306::render(uint8_t *buf)
{
    // update a portion of the display with a render area
    uint8_t cmds[] = {
        SSD1306_SET_COL_ADDR,
        this->area.start_col,
        this->area.end_col,
        SSD1306_SET_PAGE_ADDR,
        this->area.start_page,
        this->area.end_page};
    send_cmd_list(cmds, count_of(cmds));
    send_buf(buf, this->area.buflen);
}

void SSD1306::SetPixel(uint8_t *buf, int x, int y, bool on)
{
}

void SSD1306::DrawLine(uint8_t *buf, int x0, int y0, int x1, int y1, bool on)
{
}

inline int SSD1306::GetFontIndex(uint8_t ch)
{
    return 0;
}

void SSD1306::WriteChar(uint8_t *buf, int16_t x, int16_t y, uint8_t ch)
{
}

void SSD1306::WriteString(uint8_t *buf, int16_t x, int16_t y, char *str)
{
}
