#include <hw_i2c.h>
#include <string>

#include "probe.h"
#include "ssd1306.h"
#include "ky_040.h"

#define SW_K0 6
#define ENCODER_CLK 26
#define ENCODER_DT 21

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

config_master_i2c_t cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE};

init_config_SSD1306_t cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

switch_button_config_t sw_conf{
    .debounce_delay_us = 1000,
    .long_press_delay_us = 2500000};

switch_button_config_t clk_conf{
    .debounce_delay_us = 100,
};

int cursor;
int cursor_max{20};
int cursor_min{0};
int cursor_middle{(cursor_max + cursor_min) / 2};

void reset_cursor()
{
    cursor = cursor_middle;
    // printf("%*c\n", cursor, '|');
}
void increment_cursor()
{
    cursor++;
    cursor = std::min(cursor_max, std::max(cursor_min, cursor));
    // printf("%+*d\n", cursor, cursor - cursor_middle);
}
void decrement_cursor()
{
    cursor--;
    cursor = std::min(cursor_max, std::max(cursor_min, cursor));
    // printf("%+*d\n", cursor, cursor - cursor_middle);
}
void call_back(uint gpio, uint32_t event_mask);
SwitchButtonWithIRQ sw = SwitchButtonWithIRQ(SW_K0, &call_back, sw_conf);
KY040_IRQ encoder_clk = KY040_IRQ(ENCODER_CLK, ENCODER_DT, &call_back, clk_conf);

void call_back(uint gpio, uint32_t event_mask)
{
    SwitchButtonEvent sw_event = sw.get_event();
    switch (sw_event)
    {
    case SwitchButtonEvent::PUSHED:
        reset_cursor();
        break;
    case SwitchButtonEvent::RELEASED_AFTER_LONG_TIME:
        break;
    case SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME:
        break;

    default:
        break;
    }
    EncoderEvent encoder_event = encoder_clk.get_event();
    switch (encoder_event)
    {
    case EncoderEvent::INCREMENT:
        increment_cursor();
        break;
    case EncoderEvent::DECREMENT:
        decrement_cursor();
        break;
    default:
        break;
    }
}

void display_text(SSD1306 *display)
{
    text_config_t txt_conf = {
        .font = font_12x16,
        .wrap = false};
    display->set_text_config(txt_conf);

    display->clear_pixel_buffer_and_show_full_screen();
    while (true)
    {
        sprintf(display->text_buffer, "cursor:%-+2d\r", cursor - cursor_middle);
        display->print_text();
        display->show();
    }
}

int main()
{
    stdio_init_all();
    hw_I2C_master master = hw_I2C_master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    display_text(&display);

    return 0;
}
