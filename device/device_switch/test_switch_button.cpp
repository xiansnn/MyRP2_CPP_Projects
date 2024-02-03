#include "switch_button.h"
#include "probe.h"

#define SW_K0 6
#define ENCODER_CLK 26

Probe pr_D5 = Probe(5);
Probe pr_D4 = Probe(4);

switch_button_config_t central_switch_conf{
    .debounce_delay_us = 1000,
    .long_push_delay_us = 2500000};
switch_button_config_t encoder_clk_conf{
    .debounce_delay_us = 100,
};

int main()
{
    stdio_init_all();
    SwitchButton central_switch = SwitchButton(SW_K0, central_switch_conf);
    SwitchButton encoder_clk = SwitchButton(ENCODER_CLK, encoder_clk_conf);

    while (true)
    {
        pr_D4.hi();
        SwitchButtonEvent sw_event = central_switch.get_event();
        pr_D4.lo();

        if (sw_event != SwitchButtonEvent::NOOP)
        {
            printf("SW event(%d)\n", sw_event);
        }

        pr_D5.hi();
        SwitchButtonEvent clk_event = encoder_clk.get_event();
        pr_D5.lo();

        if (clk_event != SwitchButtonEvent::NOOP)
        {
            printf("CLK event(%d)\n", clk_event);
        }
    }

    return 0;
}
