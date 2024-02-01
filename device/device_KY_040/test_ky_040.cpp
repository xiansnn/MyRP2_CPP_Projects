#include "probe.h"
#include "ky_040.h"

#include <string>

#include "controlled_value.h"

#define SW_K0 6
#define ENCODER_CLK 26
#define ENCODER_DT 21
#define MAX_VALUE 20
#define MIN_VALUE 0

Probe pr_D5 = Probe(5);
Probe pr_D4 = Probe(4);

switch_button_config_t sw_conf{
    .debounce_delay_us = 1000,
    .long_press_delay_us = 500000};

switch_button_config_t clk_conf{
    .debounce_delay_us = 100,
};

int main()
{
    stdio_init_all();
    SwitchButton sw = SwitchButton(SW_K0, sw_conf);
    KY040 encoder = KY040(ENCODER_CLK,ENCODER_DT, clk_conf);
    ControlledValue val = ControlledValue(MIN_VALUE, MAX_VALUE);

    while (true)
    {
        pr_D4.hi();
        SwitchButtonEvent sw_event = sw.get_event();
        pr_D4.lo();

        switch (sw_event)
        {
        case SwitchButtonEvent::PUSHED:
            val.reset_value();
            printf("%2d %+*c\n", val.get_value(), val.get_value(), '|');
            break;
        case SwitchButtonEvent::RELEASED_AFTER_LONG_TIME:
            printf("%2d %+*c\n", val.get_value(), val.get_value(), '|');
            break;
        case SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME:
            printf("%2d %+*c\n", val.get_value(), val.get_value(), '|');
            break;

        default:
            break;
        }

        pr_D5.hi();
        EncoderEvent encoder_event = encoder.get_event();

        switch (encoder_event)
        {
        case EncoderEvent::INCREMENT:
            val.increment_value();
            printf("%2d %+*c\n", val.get_value(), val.get_value(), '|');
            break;
        case EncoderEvent::DECREMENT:
            val.decrement_value();
            printf("%2d %+*c\n", val.get_value(), val.get_value(), '|');
            break;
        default:
            break;
        }
        pr_D5.lo();

        sleep_ms(5);
        /**
         * caution : depending on the period of the scan, the value of the DT pahse of the encoder may be red when the true value has already changed.
        */
    }

    return 0;
}
