#include "switch_button.h"

#define SW_K0 6
#define ENCODER_CLK 26

switch_button_config_t sw_conf {
    .debounce_delay_us = 1000,
    .long_press_delay_us = 2500000
};
switch_button_config_t clk_conf {
    .debounce_delay_us = 100,
};

int main(){
    stdio_init_all();
    SwitchButton k0 = SwitchButton(SW_K0, sw_conf);
    SwitchButton clk = SwitchButton(ENCODER_CLK, clk_conf);

    while (true)
    {
        SwitchButtonEvent k0_event = k0.get_event();

        if (k0_event != SwitchButtonEvent::NOOP)
        {
            printf("SW event# : %d\n", k0_event);
        }
        SwitchButtonEvent clk_event = clk.get_event();

        if (clk_event != SwitchButtonEvent::NOOP)
        {
            printf("CLK event# : %d\n", clk_event);
        }
        // sleep_ms(1);
    }

    return 0;
}
