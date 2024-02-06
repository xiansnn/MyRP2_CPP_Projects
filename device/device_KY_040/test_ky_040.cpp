#include "probe.h"
#include "ky_040.h"

#include <string>
#include <vector>

#include "controlled_value.h"

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21

Probe pr_D5 = Probe(5);
Probe pr_D4 = Probe(4);
Probe pr_D3 = Probe(3);

switch_button_config_t central_switch_conf{
    .debounce_delay_us = 2000,
    .long_release_delay_us = 3000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};

switch_button_config_t encoder_clk_conf{
    .debounce_delay_us = 1000,
};

void shared_irq_call_back(uint gpio, uint32_t event_mask);
KY040 encoder = KY040(ENCODER_CLK_GPIO, ENCODER_DT_GPIO, shared_irq_call_back,
                      encoder_clk_conf);

void shared_irq_call_back(uint gpio, uint32_t event_mask)
{
    pr_D3.hi();
    switch (gpio)
    {
    case ENCODER_CLK_GPIO:
        gpio_set_irq_enabled_with_callback(ENCODER_CLK_GPIO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false, shared_irq_call_back);
        if (event_mask == GPIO_IRQ_EDGE_FALL)
            encoder.interrupt_service_routine();
        gpio_set_irq_enabled_with_callback(ENCODER_CLK_GPIO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, shared_irq_call_back);
        break;

    default:
        printf("unknown IRQ\n");
        break;
    }
    pr_D3.lo();
};

int display_value(ControlledValue *val)
{
#define MAX_WIDTH 80.
    float a = (MAX_WIDTH - 1.) / (val->get_max_value() - val->get_min_value());
    float b = 1 - a * val->get_min_value();
    return a * val->get_value() + b;
};
static int current_index = 0;
ControlledValue *next_cntrl_value_index(std::vector<ControlledValue *> values)
{
    current_index++;
    current_index = current_index % values.size();
    return values[current_index];
}

int main()
{
    stdio_init_all();
    SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_GPIO, central_switch_conf);
    ControlledValue val1 = ControlledValue(-20, +20);
    ControlledValue val2 = ControlledValue(5, 20);
    ControlledValue val3 = ControlledValue(-20, -5);

    std::vector<ControlledValue *> cntrl_values = {&val1, &val2, &val3};

    ControlledValue *current_cntrl_value = encoder.set_cntrl_value(cntrl_values[0]);

    while (true)
    {

        if (current_cntrl_value->has_changed)
        {
            pr_D5.hi();
            printf("LOOP[%d]: %2d %*c\n", current_index, current_cntrl_value->get_value(), display_value(current_cntrl_value), '|');
            current_cntrl_value->clear_change_flag();
            pr_D5.lo();
        }

        pr_D4.hi();
        SwitchButtonEvent sw_event = central_switch.get_sample_event();
        switch (sw_event)
        {
        case SwitchButtonEvent::PUSH:
            // next_cntrl_value_index(cntrl_values);
            printf("............push\n");
            break;
        case SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME:
            current_cntrl_value->reset_value();
            printf(".......short.rel\n");
            break;
        case SwitchButtonEvent::LONG_PUSH:
            current_cntrl_value = encoder.set_cntrl_value(next_cntrl_value_index(cntrl_values));
            printf(".......long.push\n");
            break;
        case SwitchButtonEvent::RELEASED_AFTER_LONG_TIME:
            printf(".......long..rel\n");
            break;
        default:
            break;
        }
        pr_D4.lo();

        sleep_ms(20);
    }

    return 0;
}
