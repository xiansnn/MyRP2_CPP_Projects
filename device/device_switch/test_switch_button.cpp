#include "switch_button.h"
#include "probe.h"
#include <map>
#include <string>

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26

switch_button_config_t central_switch_conf{
    .debounce_delay_us = 1000,
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

int main()
{
    stdio_init_all();
    SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_GPIO, central_switch_conf);
    SwitchButton encoder_clk = SwitchButton(ENCODER_CLK_GPIO, encoder_clk_conf);

    while (true)
    {
        SwitchButtonEvent sw_event = central_switch.process_sample_event();
        if (sw_event != SwitchButtonEvent::NOOP)
            printf("SW event(%s)\n", sw_events[sw_event].c_str());
        SwitchButtonEvent clk_event = encoder_clk.process_sample_event();
        if (clk_event != SwitchButtonEvent::NOOP)
            printf("CLK event(%s)\n", sw_events[clk_event].c_str());
    }

    return 0;
}
