#include "switch_button.h"
#include "probe.h"
#include <map>
#include <string>

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26

Probe pr_D1 = Probe(1);
Probe pr_D2 = Probe(2);
Probe pr_D3 = Probe(3);
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

switch_button_config_t central_switch_conf{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 2000000,
    .long_push_delay_us = 1000000};

switch_button_config_t encoder_clk_conf{
    .debounce_delay_us = 1000,
};

std::map<SwitchButtonEvent, std::string> sw_events{
    {SwitchButtonEvent::NOOP, "NOOP"},
    {SwitchButtonEvent::PUSH, "PUSH"},
    {SwitchButtonEvent::LONG_PUSH, "LONG_PUSH"},
    {SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME, "RELEASED_AFTER_SHORT_TIME"},
    {SwitchButtonEvent::RELEASED_AFTER_LONG_TIME, "RELEASED_AFTER_LONG_TIME"},
};

void sw_call_back(uint gpio, uint32_t event_mask);
SwitchButtonWithIRQ sw = SwitchButtonWithIRQ(CENTRAL_SWITCH_GPIO, &sw_call_back, central_switch_conf);
SwitchButtonWithIRQ clk = SwitchButtonWithIRQ(ENCODER_CLK_GPIO, &sw_call_back, encoder_clk_conf);

void sw_call_back(uint gpio, uint32_t event_mask)
{
    pr_D3.hi();
    if (gpio == CENTRAL_SWITCH_GPIO)
    {
        SwitchButtonEvent sw_event = sw.process_IRQ_event(event_mask);
        if (sw_event != SwitchButtonEvent::NOOP)
        {
            printf("SW event(%s) mask(%d)\n", sw_events[sw_event].c_str(), event_mask);
        }
    }
    if (gpio == ENCODER_CLK_GPIO)
    {
        clk.irq_enabled(false);
        SwitchButtonEvent clk_event = clk.process_IRQ_event(event_mask);
        if (clk_event != SwitchButtonEvent::NOOP)
        {
            pr_D4.pulse_us(1);
            printf("Encoder event(%s) mask(%d)\n", sw_events[clk_event].c_str(), event_mask);
        }
        else
        {
            pr_D5.pulse_us(1);
        }
        clk.irq_enabled(true);
    }
    pr_D3.lo();
}

;

int main()
{
    stdio_init_all();

    while (true)
    {
        pr_D1.copy(clk.is_button_active());
        pr_D2.copy(sw.is_button_active());
    }

    return 0;
}
