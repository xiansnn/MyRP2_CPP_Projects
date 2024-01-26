#include "probe.h"
#include "ky_040.h"

#include <string>

#define SW_K0 6
#define ENCODER_CLK 26
#define ENCODER_DT 21

Probe pr_D5 = Probe(5);
Probe pr_D4 = Probe(4);

switch_button_config_t sw_conf{
    .debounce_delay_us = 1000,
    .long_press_delay_us = 2500000};

switch_button_config_t clk_conf{
    .debounce_delay_us = 100,
};
int cursor;
int cursor_max{20};
int cursor_min{0};
int cursor_zero{(cursor_max + cursor_min) / 2};

void reset_cursor()
{
    cursor = cursor_zero;
    printf("%*c\n", cursor, '|');
}
void increment_cursor()
{
    cursor++;
    cursor = std::min(cursor_max, std::max(cursor_min, cursor));
    printf("%+*d\n", cursor, cursor - cursor_zero);
}
void decrement_cursor()
{
    cursor--;
    cursor = std::min(cursor_max, std::max(cursor_min, cursor));
    printf("%+*d\n", cursor, cursor - cursor_zero);
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

int main()
{
    stdio_init_all();

    while (true)
    {
    }

    return 0;
}
