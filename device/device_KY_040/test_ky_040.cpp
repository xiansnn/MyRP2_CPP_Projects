#include "probe.h"
#include "ky_040.h"

#include <string>

#include "controlled_value.h"

#define SW_K0 6
#define ENCODER_CLK 26
#define ENCODER_DT 21
#define MAX_VALUE 20
#define MIN_VALUE -20

Probe pr_D5 = Probe(5);
Probe pr_D4 = Probe(4);
Probe pr_D1 = Probe(1);

switch_button_config_t sw_conf{
    .debounce_delay_us = 1000,
    .long_release_delay_us = 3000000,
    .long_push_delay_us = 500000};

switch_button_config_t clk_conf{
    .debounce_delay_us = 1000,
};

void call_back(uint gpio, uint32_t event_mask);

KY040_IRQ encoder = KY040_IRQ(ENCODER_CLK, ENCODER_DT, &call_back, clk_conf);
ControlledValue val = ControlledValue(MIN_VALUE, MAX_VALUE);

void call_back(uint gpio, uint32_t event_mask)
{
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

float a = 79. / (MAX_VALUE - MIN_VALUE);
float b = 1 - a * MIN_VALUE;
int get_tab(int val) { return a * val + b; };
int main()
{
    stdio_init_all();
    SwitchButton sw = SwitchButton(SW_K0, sw_conf);

    while (true)
    {
        if (val.has_changed)
        {
            printf("LOOP: %2d %*c\n", val.get_value(), get_tab(val.get_value()), '|');
            val.clear_change_flag();
        }

        pr_D4.hi();
        SwitchButtonEvent sw_event = sw.get_event();

        switch (sw_event)
        {
        case SwitchButtonEvent::PUSH:
            printf("............push\n");
            break;
        case SwitchButtonEvent::LONG_PUSH:
            printf(".......long.push\n");
            break;
        case SwitchButtonEvent::RELEASED_AFTER_LONG_TIME:
            printf(".......long..rel\n");
            break;
        case SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME:
            val.reset_value();
            printf(".......short.rel\n");
            break;
        default:
            break;
        }
        pr_D4.lo();

        sleep_ms(20);
    }

    return 0;
}
