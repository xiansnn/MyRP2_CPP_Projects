#include "switch_button.h"
#include "probe.h"

#define SW_K0 6
#define ENCODER_CLK 26

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
    SwitchButton clk = SwitchButton(ENCODER_CLK, clk_conf);

    while (true)
    {
        pr_D4.hi();
        SwitchButtonEvent sw_event = sw.get_event();
        pr_D4.lo();

        if (sw_event != SwitchButtonEvent::NOOP)
        {
            printf("SW event(%d)\n", sw_event);
        }

        pr_D5.hi();
        SwitchButtonEvent clk_event = clk.get_event();
        pr_D5.lo();

        if (clk_event != SwitchButtonEvent::NOOP)
        {
            printf("CLK event(%d)\n", clk_event);
        }
    }

    return 0;
}
