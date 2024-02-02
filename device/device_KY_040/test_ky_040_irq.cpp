#include "probe.h"
#include "ky_040.h"
#include "controlled_value.h"

#include <string>

#define SW_K0 6
#define ENCODER_CLK 26
#define ENCODER_DT 21
#define MAX_VALUE 20
#define MIN_VALUE -5

Probe pr_D5 = Probe(5);
Probe pr_D4 = Probe(4);

switch_button_config_t sw_conf{
    .debounce_delay_us = 1000,
    .long_press_delay_us = 2500000};

switch_button_config_t clk_conf{
    .debounce_delay_us = 100,
};

void call_back(uint gpio, uint32_t event_mask);
SwitchButtonWithIRQ sw = SwitchButtonWithIRQ(SW_K0, &call_back, sw_conf);
KY040_IRQ encoder = KY040_IRQ(ENCODER_CLK, ENCODER_DT, &call_back, clk_conf);
ControlledValue val = ControlledValue(MIN_VALUE, MAX_VALUE);


void call_back(uint gpio, uint32_t event_mask)
{
    pr_D4.hi();
    SwitchButtonEvent sw_event = sw.get_event();
    switch (sw_event)
    {
    case SwitchButtonEvent::PUSH:
        val.reset_value();
        break;
    case SwitchButtonEvent::RELEASED_AFTER_LONG_TIME:
        break;
    case SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME:
        break;

    default:
        break;
    }
    pr_D4.lo();
    pr_D5.hi();
    EncoderEvent encoder_event = encoder.get_encoder_event();
    switch (encoder_event)
    {
    case EncoderEvent::INCREMENT:
        val.increment_value();
        break;
    case EncoderEvent::DECREMENT:
        val.decrement_value();
        break;
    default:
        break;
    }
    pr_D5.lo();
}

int main()
{
    stdio_init_all();

    while (true)
    {
        if (val.has_changed)
        {
            printf("LOOP: %2d %*c\n",val.get_value(), std::abs( val.get_value()), '|');
            val.clear_change_flag();
        }
            sleep_ms(20);
    }

    return 0;
}
