#include "ssd1306.h"
#include <string.h>

void SSD1306::send_cmd(uint8_t cmd)
{
    // I2C write process expects a control byte followed by data
    // this "data" can be a command or data to follow up a command
    this->i2c_master->single_byte_write(this->config.i2c_address, I2C_CMD_FLAG, cmd);
}

void SSD1306::send_cmd_list(uint8_t *cmd_list, int cmd_list_size)
{
    for (int i = 0; i < cmd_list_size; i++)
        this->send_cmd(cmd_list[i]);
}

void SSD1306::send_buf(uint8_t buffer[], size_t buffer_size)
{
    this->i2c_master->burst_byte_write(this->config.i2c_address, I2C_DATA_FLAG, buffer, buffer_size);
}

SSD1306::SSD1306(hw_I2C_master *master, init_config_SSD1306_t init_config)
{
    this->i2c_master = master;
    this->config = init_config;
    this->init();
}

render_area_t SSD1306::compute_render_area(uint8_t start_col, uint8_t end_col, uint8_t start_page, uint8_t end_page)
{
    render_area_t area;
    area.start_col = start_col;
    area.end_col = end_col;
    area.start_page = start_page;
    area.end_page = end_page;
    // this->calc_render_area_buflen();
    area.buflen = (area.end_col - area.start_col + 1) * (area.end_page - area.start_page + 1);
    return area;
}

void SSD1306::set_display_from_RAM()
{
    this->send_cmd(SSD1306_SET_RAM_DISPLAY);
}

void SSD1306::set_all_pixel_ON()
{
    this->send_cmd(SSD1306_SET_DISPLAY_ON);
}

void SSD1306::set_inverse_color(bool inverse)
{
    if (inverse)
        this->send_cmd(SSD1306_SET_INV_DISP);
    else
        this->send_cmd(SSD1306_SET_NORM_DISP);
}

void SSD1306::set_display_OFF()
{
    this->send_cmd(SSD1306_SET_DISPLAY_SLEEP);
}

void SSD1306::set_display_ON()
{
    this->send_cmd(SSD1306_SET_DISPLAY_NORMAL_ON);
}

void SSD1306::fill_GDDRAM(uint8_t pattern, render_area_t area)
{
    uint8_t image[SSD1306_BUF_LEN];
    memset(image, pattern, SSD1306_BUF_LEN);
    this->show_render_area(HORIZONTAL_ADDRESSING_MODE, image, area);
}

void SSD1306::clear_screen_buffer_and_GDDRAM()
{
    render_area_t full_screen_area = this->compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_NUM_PAGES - 1);
    memset(this->screen_buffer, 0x00, SSD1306_BUF_LEN);
    this->show_render_area(HORIZONTAL_ADDRESSING_MODE, this->screen_buffer, full_screen_area);
}

void SSD1306::init()
{
    // printf("init_display\n");
    this->set_display_OFF();
    this->init_MUX_ratio(config.mux_ratio_value);
    this->init_display_vertical_shift(config.vertical_offset);
    this->init_RAM_start_line(config.GDDRAM_start_line);
    this->init_SEG_scan_inverse_direction(config.scan_SEG_inverse_direction);
    this->init_COM_scan_inverse_direction(config.scan_COM_inverse_direction);
    this->init_COM_cfg(config.sequential_COM, config.enable_COM_L_R_remap);
    this->set_contrast(config.contrast);
    this->set_display_from_RAM();
    this->set_inverse_color(false);
    this->init_clock_frequency(config.frequency_divider, config.frequency_factor);
    this->init_charge_pump_enabled(true);
    this->set_display_ON();

    uint8_t cmds[] = {
        SSD1306_SET_MEM_MODE,   // set memory address mode 0 = horizontal, 1 = vertical, 2 = page
        0x00,                   // horizontal addressing mode
        SSD1306_SET_SCROLL_OFF, // deactivate horizontal scrolling if set. This is necessary as memory writes will corrupt if scrolling was enabled
    };

    this->send_cmd_list(cmds, count_of(cmds));
}

void SSD1306::init_MUX_ratio(uint8_t value)
{
    assert((value >= 1) & (value <= 64));
    this->send_cmd(SSD1306_SET_MUX_RATIO);
    this->send_cmd(value - 1);
}

void SSD1306::show_render_area(const uint8_t addressing_mode, uint8_t *data_buffer, const render_area_t buffer_area)
{
    assert((addressing_mode >= 0) & (addressing_mode <= 2));
    this->send_cmd(SSD1306_SET_MEM_MODE);
    this->send_cmd(addressing_mode);
    if (addressing_mode == PAGE_ADDRESSING_MODE)
    {
        // this->send_cmd(SSD1306_SET_COL_ADDR);
        // this->send_cmd(buffer_area.start_col);
        // this->send_cmd(buffer_area.end_col);
        uint8_t page_start_address = 0xB0 | buffer_area.start_page;
        this->send_cmd(page_start_address);
        uint8_t column_start_LO_address = 0x0F & buffer_area.start_col;
        this->send_cmd(column_start_LO_address);
        uint8_t column_start_HI_address = (((0xF0) & buffer_area.start_col) >> 4) | 0x10;
        this->send_cmd(column_start_HI_address);
        this->send_buf(data_buffer, buffer_area.buflen);
    }
    else
    {
        this->send_cmd(SSD1306_SET_COL_ADDR);
        this->send_cmd(buffer_area.start_col);
        this->send_cmd(buffer_area.end_col);
        this->send_cmd(SSD1306_SET_PAGE_ADDR);
        this->send_cmd(buffer_area.start_page);
        this->send_cmd(buffer_area.end_page);
        this->send_buf(data_buffer, buffer_area.buflen);
    }
}

void SSD1306::init_display_vertical_shift(uint8_t value)
{
    assert((value >= 0) & (value <= 63));
    this->send_cmd(SSD1306_SET_DISP_OFFSET);
    this->send_cmd(value);
}

void SSD1306::init_RAM_start_line(uint8_t value)
{
    assert((value >= 0) & (value <= 63));
    this->send_cmd(SSD1306_SET_DISP_START_LINE);
    this->send_cmd(value);
}

void SSD1306::init_SEG_scan_inverse_direction(bool inverse)
{
    if (inverse == true)
        this->send_cmd(SSD1306_SET_SEG_REMAP);
    else
        this->send_cmd(SSD1306_SET_SEG_NO_REMAP);
}

void SSD1306::init_COM_scan_inverse_direction(bool inverse)
{
    if (inverse == true)
        this->send_cmd(SSD1306_SET_COM_OUT_DIR_REMAP);
    else
        this->send_cmd(SSD1306_SET_COM_OUT_DIR);
}

void SSD1306::init_COM_cfg(bool sequential_COM, bool enable_COM_L_R_remap)
{
    uint8_t value = 0x02;
    this->send_cmd(SSD1306_SET_COM_PIN_CFG);
    if (!sequential_COM)
    {
        value = value | 0x10;
    }
    if (enable_COM_L_R_remap)
    {
        value = value | 0x20;
    }
    this->send_cmd(value);
}

void SSD1306::set_contrast(uint8_t value)
{
    assert((value >= 0) & (value <= 255));
    this->send_cmd(SSD1306_SET_CONTRAST);
    this->send_cmd(value);
}

void SSD1306::init_clock_frequency(uint8_t divide_ratio, uint8_t frequency_factor)
{
    assert((divide_ratio >= 1) & (divide_ratio <= 16));
    assert((frequency_factor >= 0) & (frequency_factor <= 15));
    uint8_t value = (frequency_factor << 4) | (divide_ratio - 1);
    this->send_cmd(SSD1306_SET_DISP_CLK_DIV);
    this->send_cmd(value);
}

void SSD1306::init_charge_pump_enabled(bool enabled)
{
    this->send_cmd(SSD1306_SET_PRECHARGE);
    this->send_cmd(0x22); // 0xF1
    this->send_cmd(SSD1306_SET_VCOM_DESEL);
    this->send_cmd(0x20); // 0x30
    uint8_t value;
    this->send_cmd(SSD1306_SET_CHARGE_PUMP);
    if (enabled)
        value = 0x14;
    else
        value = 0x10;
    this->send_cmd(value);
}

void SSD1306::horizontal_scroll(bool on, config_scroll_t scroll_data)
{ // configure horizontal scrolling by 1 column
    // send_cmd(SSD1306_SET_SCROLL_OFF);
    uint8_t cmds[8];
    if (scroll_data.scroll_H_to_right)
        cmds[0] = SSD1306_SET_R_HORIZ_SCROLL;
    else
        cmds[0] = SSD1306_SET_L_HORIZ_SCROLL;
    cmds[1] = 0x00;
    cmds[2] = scroll_data.scroll_H_start_page;
    cmds[3] = scroll_data.time_frame_interval;
    cmds[4] = scroll_data.scroll_H_end_page;
    cmds[5] = 0x00;
    cmds[6] = 0xFF;
    if (on)
        cmds[7] = SSD1306_SET_SCROLL_ON;
    else
        cmds[7] = SSD1306_SET_SCROLL_OFF;
    this->send_cmd_list(cmds, count_of(cmds));
}

void SSD1306::vertical_scroll(bool on, config_scroll_t scroll_data)
{
    // send_cmd(SSD1306_SET_SCROLL_OFF);
    uint8_t cmds[7];
    if (scroll_data.scroll_H_to_right)
        cmds[0] = SSD1306_SET_VERTICAL_R_HORIZ_SCROLL;
    else
        cmds[0] = SSD1306_SET_VERTICAL_L_HORIZ_SCROLL;
    cmds[1] = 0x00;
    cmds[2] = scroll_data.scroll_H_start_page;
    cmds[3] = scroll_data.time_frame_interval;
    cmds[4] = scroll_data.scroll_H_end_page;
    cmds[5] = scroll_data.vertical_scrolling_offset;
    if (on)
        cmds[6] = SSD1306_SET_SCROLL_ON;
    else
        cmds[6] = SSD1306_SET_SCROLL_OFF;
    this->send_cmd_list(cmds, count_of(cmds));
}

// void SSD1306::render(uint8_t *buf)
// {
//     // update a portion of the display with a render area
//     uint8_t cmds[] = {
//         SSD1306_SET_COL_ADDR,
//         this->render_area.start_col,
//         this->render_area.end_col,
//         SSD1306_SET_PAGE_ADDR,
//         this->render_area.start_page,
//         this->render_area.end_page};
//     this->send_cmd_list(cmds, count_of(cmds));
//     this->send_buf(buf, this->render_area.buflen);
// }

void SSD1306::SetPixel(uint8_t *buf, int x, int y, bool on)
{
    assert(x >= 0 && x < SSD1306_WIDTH && y >= 0 && y < SSD1306_HEIGHT);

    // The calculation to determine the correct bit to set depends on which address
    // mode we are in. This code assumes horizontal

    // The video ram on the SSD1306 is split up in to 8 rows, one bit per pixel.
    // Each row is 128 long by 8 pixels high, each byte vertically arranged, so byte 0 is x=0, y=0->7,
    // byte 1 is x = 1, y=0->7 etc

    // This code could be optimised, but is like this for clarity. The compiler
    // should do a half decent job optimising it anyway.

    const int BytesPerRow = SSD1306_WIDTH; // x pixels, 1bpp, but each row is 8 pixel high, so (x / 8) * 8

    int byte_idx = (y / 8) * BytesPerRow + x;
    uint8_t byte = buf[byte_idx];

    if (on)
        byte |= 1 << (y % 8);
    else
        byte &= ~(1 << (y % 8));

    buf[byte_idx] = byte;
}

void SSD1306::DrawLine(uint8_t *buf, int x0, int y0, int x1, int y1, bool on)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (true)
    {
        SetPixel(buf, x0, y0, on);
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
