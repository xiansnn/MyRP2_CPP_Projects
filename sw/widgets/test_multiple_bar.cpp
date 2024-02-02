#include <hw_i2c.h>
// #include <string>

#include "probe.h"
#include "ssd1306.h"
#include "../device_KY_040/ky_040.h"
#include "widget_bar.h"

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
    .long_press_delay_us = 1000000};

switch_button_config_t clk_conf{
    .debounce_delay_us = 100,
};

bar_widget_config_t bar_conf{
    .level_max = -5,
    .level_min = -15,
    .width = 128,
    .height = 8,
    .draw_border = true,
    .draw_value = true,
    .font = font_5x8};

bool next_bar{false};

void call_back(uint gpio, uint32_t event_mask);
SwitchButtonWithIRQ sw = SwitchButtonWithIRQ(SW_K0, &call_back, sw_conf);
KY040_IRQ encoder_clk = KY040_IRQ(ENCODER_CLK, ENCODER_DT, &call_back, clk_conf);

Bar bar = Bar(bar_conf);

void call_back(uint gpio, uint32_t event_mask)
{
    SwitchButtonEvent sw_event = sw.get_event();
    switch (sw_event)
    {
    case SwitchButtonEvent::PUSH:
        break;
    case SwitchButtonEvent::RELEASED_AFTER_LONG_TIME:
        bar.reset_px();
        break;
    case SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME:
        next_bar = true;
        break;

    default:
        break;
    }
    EncoderEvent encoder_event = encoder_clk.get_event();
    switch (encoder_event)
    {
    case EncoderEvent::INCREMENT:
        bar.increment_level();
        break;
    case EncoderEvent::DECREMENT:
        bar.decrement_level();
        break;
    default:
        break;
    }
};

int main(void)

{
    stdio_init_all();
    hw_I2C_master master = hw_I2C_master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    display.clear_pixel_buffer_and_show_full_screen();

    while (true)
    {
        bar_conf = {
            .level_max = -5,
            .level_min = -15,
            .width = 128,
            .height = 8,
            .draw_border = true,
            .draw_value = true,
            .font = font_5x8};
        bar.init(bar_conf);

        while (!next_bar)
        {
            pr_D4.hi();
            bar.draw();
            pr_D4.lo();
            pr_D5.hi();
            display.show(&bar, 0, 0);
            pr_D5.lo();
            sleep_ms(15);
        }
        next_bar = false;
        bar_conf = {
            .level_max = 10,
            .level_min = 0,
            .width = 128,
            .height = 8,
            .draw_border = true,
            .draw_value = true,
            .font = font_5x8};

        bar.init(bar_conf);
        while (!next_bar)
        {
            pr_D4.hi();
            bar.draw();
            pr_D4.lo();
            pr_D5.hi();
            display.show(&bar, 0, 8);
            pr_D5.lo();
            sleep_ms(15);
        }
        next_bar = false;
        bar_conf = {
            .level_max = 15,
            .level_min = 10,
            .width = 128,
            .height = 8,
            .draw_border = true,
            .draw_value = true,
            .font = font_5x8};

        bar.init(bar_conf);
        while (!next_bar)
        {
            pr_D4.hi();
            bar.draw();
            pr_D4.lo();
            pr_D5.hi();
            display.show(&bar, 0, 16);
            pr_D5.lo();
            sleep_ms(15);
        }
        next_bar = false;
        bar_conf = {
            .level_max = 20,
            .level_min = 0,
            .width = 128,
            .height = 8,
            .draw_border = true,
            .draw_value = true,
            .font = font_5x8};

        bar.init(bar_conf);
        while (!next_bar)
        {
            pr_D4.hi();
            bar.draw();
            pr_D4.lo();
            pr_D5.hi();
            display.show(&bar, 0, 24);
            pr_D5.lo();
            sleep_ms(15);
        }
        next_bar = false;
        bar_conf = {
            .level_max = +15,
            .level_min = -15,
            .width = 128,
            .height = 8,
            .draw_border = true,
            .draw_value = true,
            .font = font_5x8};

        bar.init(bar_conf);
        while (!next_bar)
        {
            pr_D4.hi();
            bar.draw();
            pr_D4.lo();
            pr_D5.hi();
            display.show(&bar, 0, 32);
            pr_D5.lo();
            sleep_ms(15);
        }
        next_bar = false;
        bar_conf = {
            .level_max = +4,
            .level_min = -20,
            .width = 128,
            .height = 8,
            .draw_border = true,
            .draw_value = true,
            .font = font_5x8};

        bar.init(bar_conf);
        while (!next_bar)
        {
            pr_D4.hi();
            bar.draw();
            pr_D4.lo();
            pr_D5.hi();
            display.show(&bar, 0, 40);
            pr_D5.lo();
            sleep_ms(15);
        }
        next_bar = false;
        bar_conf = {
            .level_max = +15,
            .level_min = -15,
            .width = 128,
            .height = 8,
            .draw_border = true,
            .draw_value = true,
            .font = font_5x8};

        bar.init(bar_conf);
        while (!next_bar)
        {
            pr_D4.hi();
            bar.draw();
            pr_D4.lo();
            pr_D5.hi();
            display.show(&bar, 0, 48);
            pr_D5.lo();
            sleep_ms(15);
        }
        next_bar = false;
    bar_conf = {
            .level_max = +20,
            .level_min = -5,
            .width = 128,
            .height = 8,
            .draw_border = true,
            .draw_value = true,
            .font = font_5x8};

        bar.init(bar_conf);
        while (!next_bar)
        {
            pr_D4.hi();
            bar.draw();
            pr_D4.lo();
            pr_D5.hi();
            display.show(&bar, 0, 56);
            pr_D5.lo();
            sleep_ms(15);
        }
        next_bar = false;

    }
    return 0;
}
