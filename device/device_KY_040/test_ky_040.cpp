#include "probe.h"
#include "ky_040.h"

#include <string>
#include <vector>

#include "controlled_value.h"

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define ENCODER_ID 0
#define CENTRAL_SWITCH_ID 1
#define CONTROLLED_VAL1_ID 11
#define CONTROLLED_VAL2_ID 12
#define CONTROLLED_VAL3_ID 13

config_switch_button_t central_switch_conf{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 3000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};

config_switch_button_t cfg_encoder_clk{
    .debounce_delay_us = 5000,
};

void shared_irq_call_back(uint gpio, uint32_t event_mask);

KY040Encoder encoder = KY040Encoder(ENCODER_ID, ENCODER_CLK_GPIO, ENCODER_DT_GPIO, shared_irq_call_back,
                                    cfg_encoder_clk);
SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_ID, CENTRAL_SWITCH_GPIO, central_switch_conf);

void shared_irq_call_back(uint gpio, uint32_t event_mask)
{
    switch (gpio)
    {
    case ENCODER_CLK_GPIO:
        encoder.interrupt_service_routine(event_mask);
        break;

    default:
        printf("unknown IRQ\n");
        break;
    };
}

static std::vector<ControlledValue *> controlled_objects;
static int current_index = 0;

ControlledValue *next_controlled_object(std::vector<ControlledValue *> cntrl_obj)
{
    current_index++;
    current_index %= cntrl_obj.size();
    return cntrl_obj[current_index];
}

int display_value(ControlledValue *val)
{
#define MAX_WIDTH 21.
    float a = (MAX_WIDTH - 1.) / (val->get_max_value() - val->get_min_value());
    float b = 1 - a * val->get_min_value();
    return a * val->get_value() + b;
};

int main()
{
    stdio_init_all();

    ControlledValue val1 = ControlledValue(CONTROLLED_VAL1_ID, -10, +10);
    val1.add_controller(&encoder);
    controlled_objects.push_back(&val1);

    ControlledValue val2 = ControlledValue(CONTROLLED_VAL2_ID, 5, 25);
    val2.add_controller(&encoder);
    controlled_objects.push_back(&val2);

    ControlledValue val3 = ControlledValue(CONTROLLED_VAL3_ID, -25, -5);
    val3.add_controller(&encoder);
    controlled_objects.push_back(&val3);

    ControlledValue *current_cntrl_value = controlled_objects.front();
    encoder.set_active_controlled_object(current_cntrl_value);

    while (true)
    {

        if (current_cntrl_value->has_changed)
        {
            printf("LOOP[%d]: %2d %*c\n", current_index, current_cntrl_value->get_value(), display_value(current_cntrl_value), '|');
            current_cntrl_value->clear_change_flag();
        }
        SwitchButtonEvent sw_event = central_switch.process_sample_event();
        switch (sw_event)
        {
        case SwitchButtonEvent::PUSH:
            printf("............push\n");
            break;
        case SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME:
            current_cntrl_value->reset();
            printf(".......short.rel\n");
            break;
        case SwitchButtonEvent::LONG_PUSH:
            current_cntrl_value = next_controlled_object(controlled_objects);
            encoder.set_active_controlled_object(current_cntrl_value);
            printf(".......long.push\n");
            break;
        case SwitchButtonEvent::RELEASED_AFTER_LONG_TIME:
            printf(".......long..rel\n");
            break;
        default:
            break;
        }
        sleep_ms(20);
    }

    return 0;
}
