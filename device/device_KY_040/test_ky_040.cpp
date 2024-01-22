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

int main()
{
    stdio_init_all();
    SwitchButton sw = SwitchButton(SW_K0, sw_conf);
    KY040 encoder_clk = KY040(ENCODER_CLK, ENCODER_DT, clk_conf);

    while (true)
    {
        pr_D4.hi();
        SwitchButtonEvent sw_event = sw.get_event();
        pr_D4.lo();

        switch (sw_event)
        {
        case SwitchButtonEvent::PUSHED:
            printf("SW event(%d) PUSHED\n", sw_event);
            break;
        case SwitchButtonEvent::RELEASED_AFTER_LONG_TIME:
            printf("SW event(%d) RELEASED_AFTER_LONG_TIME\n", sw_event);
            break;
        case SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME:
            printf("SW event(%d) RELEASED_AFTER_SHORT_TIME\n", sw_event);
            break;

        default:
            break;
        }

        pr_D5.hi();
        EncoderEvent encoder_event = encoder_clk.get_event();
        pr_D5.lo();

        switch (encoder_event)
        {
        case EncoderEvent::INCREMENT:
            printf("encoder CLK event(%d) INCREMENT\n", encoder_event);
            break;
        case EncoderEvent::DECREMENT:
            printf("encoder CLK event(%d) DECREMENT\n", encoder_event);
            break;
        default:
            break;
        }
    }

    return 0;
}
