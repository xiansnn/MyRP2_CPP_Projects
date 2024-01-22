#include "switch_button.h"
#include "probe.h"

#define SW_K0 6
#define ENCODER_CLK 26

Probe pr_D5 = Probe(5);
Probe pr_D4 = Probe(4);

switch_button_config_t sw_conf{
    .debounce_delay_us = 1000,
    .long_press_delay_us = 1000000};

switch_button_config_t clk_conf{
    .debounce_delay_us = 100,
};

void sw_call_back(uint gpio, uint32_t event_mask);
SwitchButtonWithIRQ sw = SwitchButtonWithIRQ(SW_K0, &sw_call_back, sw_conf);
SwitchButtonWithIRQ clk = SwitchButtonWithIRQ(ENCODER_CLK, &sw_call_back, clk_conf);

void sw_call_back(uint gpio, uint32_t event_mask)
{
    if (gpio == SW_K0)
    {
        pr_D4.hi();
        SwitchButtonEvent sw_event = sw.get_event();
        if (sw_event != SwitchButtonEvent::NOOP)
        {
            printf("SW event(%d) mask(%d)\n", sw_event, event_mask);
        }
        pr_D4.lo();
    }
    if (gpio == ENCODER_CLK)
    {
        pr_D5.hi();
        SwitchButtonEvent clk_event = clk.get_event();
        if (clk_event != SwitchButtonEvent::NOOP)
        {
            printf("Encoder event(%d) mask(%d)\n", clk_event, event_mask);
        }
        pr_D5.lo();
    }
}

;

int main()
{
    stdio_init_all();

    while (true)
    {
    }

    return 0;
}
