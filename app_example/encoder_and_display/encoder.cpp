#include <hw_i2c.h>
#include <string>

#include "probe.h"
#include "ssd1306.h"
#include "ky_040.h"
#include "encoder.h"

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
struct bar_widget_config_t
{
    int cursor{0};
    int cursor_max{10};
    int cursor_min{0};
    uint8_t anchor_x{0};
    uint8_t anchor_y{0};
    size_t width{128};
    size_t height{8};
    int cursor_coef;
    uint8_t bar_zero;
    bool border{true};
};

bar_widget_config_t bar_widget{};

size_t next_bar{0};
bool screen_change{false};

void display_text(SSD1306 *display);
void display_bar(SSD1306 *display);

void init_bar_widget()
{
    bar_widget.cursor_coef = bar_widget.width / (bar_widget.cursor_max - bar_widget.cursor_min);
    bar_widget.width = (bar_widget.cursor_max - bar_widget.cursor_min) * bar_widget.cursor_coef;
    bar_widget.bar_zero = bar_widget.anchor_x - bar_widget.cursor_min * bar_widget.cursor_coef;
    bar_widget.cursor = 0;
}
void reset_cursor()
{
    bar_widget.cursor = 0;
};
void increment_cursor()
{
    bar_widget.cursor++;
    bar_widget.cursor = std::min(bar_widget.cursor_max, std::max(bar_widget.cursor_min, bar_widget.cursor));
};
void decrement_cursor()
{
    bar_widget.cursor--;
    bar_widget.cursor = std::min(bar_widget.cursor_max, std::max(bar_widget.cursor_min, bar_widget.cursor));
};
void next_bar()
{
    next_bar++;
    next_bar %= 3;
    screen_change = true;
};
void ack_bar_change()
{
    screen_change = false;
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
        break;
    case SwitchButtonEvent::RELEASED_AFTER_LONG_TIME:
        next_bar();
        break;
    case SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME:
        reset_cursor();
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
};

void display_text(SSD1306 *display)
{

    display->clear_pixel_buffer_and_show_full_screen();
    text_config_t txt_conf = {
        .font = font_12x16,
        .wrap = false};
    display->set_text_config(txt_conf);

    while (!screen_change)
    {
        display->clear_text_buffer();
        sprintf(display->text_buffer, "cursor:%d", bar_widget.cursor);
        display->print_text();
        display->show();
    }
    ack_bar_change();
};

void display_bar(SSD1306 *display)
{

    display->clear_pixel_buffer_and_show_full_screen();
    int w_coef = bar_widget.width / (bar_widget.cursor_max - bar_widget.cursor_min);
    size_t bar_size = 0;

    text_config_t txt_conf = {
        .font = font_12x16,
        .wrap = false};
    display->set_text_config(txt_conf);

    while (!screen_change)
    {
        display->clear_text_buffer();
        sprintf(display->text_buffer, "cursor:%d", bar_widget.cursor);
        display->print_text();

        bar_size = std::abs(bar_widget.cursor * w_coef);

        display->rect(bar_widget.anchor_x, bar_widget.anchor_y, bar_widget.width, bar_widget.height, true, Framebuffer_color::black);
        if (bar_widget.border)
            display->rect(bar_widget.anchor_x, bar_widget.anchor_y, bar_widget.width, bar_widget.height);

        if (bar_widget.cursor == 0)
            display->rect(bar_widget.bar_zero, bar_widget.anchor_y, 1, bar_widget.height, true);

        else if (bar_widget.cursor > 0)
            display->rect(bar_widget.bar_zero, bar_widget.anchor_y, bar_size, bar_widget.height, true);

        else
            display->rect(bar_widget.bar_zero - bar_size, bar_widget.anchor_y, bar_size, bar_widget.height, true);

        display->show();
    }
    ack_bar_change();
};

int main()
{
    stdio_init_all();
    hw_I2C_master master = hw_I2C_master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    while (true)
    {
        bar_widget.cursor_max = 10;
        bar_widget.cursor_min = 0;
        display_text(&display);

        bar_widget.cursor_max = 10;
        bar_widget.cursor_min = 0;
        bar_widget.anchor_x = 0;
        bar_widget.anchor_y = 20;
        bar_widget.height = 8;
        bar_widget.width = 120;
        init_bar_widget();
        display_bar(&display);

        bar_widget.cursor_max = 4;
        bar_widget.cursor_min = -20;
        bar_widget.anchor_x = 0;
        bar_widget.anchor_y = 20;
        bar_widget.height = 8;
        bar_widget.width = 120;
        init_bar_widget();
        display_bar(&display);

        bar_widget.cursor_max = 10;
        bar_widget.cursor_min = 0;
        bar_widget.anchor_x = 0;
        bar_widget.anchor_y = 32;
        bar_widget.height = 5;
        bar_widget.width = 120;
        bar_widget.border = false;
        init_bar_widget();
        display_bar(&display);
    }
    return 0;
}
