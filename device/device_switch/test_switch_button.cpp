#include "switch_button.h"
#include "probe.h"
#include <map>
#include <string>

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define CENTRAL_SWITCH_ID 1
#define ENCODER_ID 0

config_switch_button_t cfg_central_switch{
    .debounce_delay_us = 1000,
    .long_release_delay_us = 2000000,
    .long_push_delay_us = 1000000};
config_switch_button_t cfg_encoder_clk{
    .debounce_delay_us = 1000,
};

std::map<SwitchButtonEvent, std::string> sw_button_events{
    {SwitchButtonEvent::NOOP, "NOOP"},
    {SwitchButtonEvent::PUSH, "PUSH"},
    {SwitchButtonEvent::LONG_PUSH, "LONG_PUSH"},
    {SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME, "RELEASED_AFTER_SHORT_TIME"},
    {SwitchButtonEvent::RELEASED_AFTER_LONG_TIME, "RELEASED_AFTER_LONG_TIME"},
};

int main()
{
    stdio_init_all();
    SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_ID,CENTRAL_SWITCH_GPIO, cfg_central_switch);
    SwitchButton encoder_clk = SwitchButton(ENCODER_ID, ENCODER_CLK_GPIO, cfg_encoder_clk);

    while (true)
    {
        SwitchButtonEvent sw_event = central_switch.process_sample_event();
        if (sw_event != SwitchButtonEvent::NOOP)
            printf("SW event(%s)\n", sw_button_events[sw_event].c_str());
        SwitchButtonEvent clk_event = encoder_clk.process_sample_event();
        if (clk_event != SwitchButtonEvent::NOOP)
            printf("CLK event(%s)\n", sw_button_events[clk_event].c_str());
    }

    return 0;
}
