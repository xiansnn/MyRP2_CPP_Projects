#include "probe.h"
#include "ky_040.h"

#include <string>

#include "controlled_value.h"

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21


Probe pr_D5 = Probe(5);
Probe pr_D4 = Probe(4);


switch_button_config_t central_switch_conf{
    .debounce_delay_us = 1000,
    .long_release_delay_us = 3000000,
    .long_push_delay_us = 500000};

switch_button_config_t encoder_clk_conf{
    .debounce_delay_us = 1000,
};

void shared_irq_call_back(uint gpio, uint32_t event_mask);
KY040_IRQ encoder = KY040_IRQ(ENCODER_CLK_GPIO, ENCODER_DT_GPIO, shared_irq_call_back,
                              encoder_clk_conf);

void shared_irq_call_back(uint gpio, uint32_t event_mask)
{
    switch (gpio)
    {
    case ENCODER_CLK_GPIO:
        encoder.process_encoder_event();
        break;

    default:
        printf("unknown IRQ\n");
        break;
    }
};

int get_tab(ControlledValue* val)
{
#define MAX_TAB_WIDTH 80.
    float a = (MAX_TAB_WIDTH - 1.) / (val->get_max_value() - val->get_min_value());
    float b = 1 - a * val->get_min_value();
    return a * val->get_value() + b;
};

void shared_irq_call_back(uint gpio, uint32_t event_mask);
int main()
{
    stdio_init_all();
    SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_GPIO, central_switch_conf);
    ControlledValue val = ControlledValue(-20, +20);
    ControlledValue val2 = ControlledValue(5,20);
    ControlledValue val3 = ControlledValue(-20,-5);
    encoder.add_cntrl_value(&val3);
    ControlledValue* current_cntrl_value = encoder.get_current_cntrl_value();

    while (true)
    {
        if (current_cntrl_value->has_changed)
        {
            pr_D5.hi();
            printf("LOOP: %2d %*c\n", current_cntrl_value->get_value(), get_tab(current_cntrl_value), '|');
            current_cntrl_value->clear_change_flag();
            pr_D5.lo();
        }

        pr_D4.hi();
        SwitchButtonEvent sw_event = central_switch.get_event();

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
            current_cntrl_value->reset_value();
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
