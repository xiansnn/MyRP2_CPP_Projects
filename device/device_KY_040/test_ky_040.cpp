#include "probe.h"
#include "ky_040.h"

#include <string>
#include <vector>

#include "controlled_value.h"
#include "print_console_line.h"
#include "focus_manager.h"

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define ENCODER_ID 11
#define CENTRAL_SWITCH_ID 12
#define FOCUS_MANAGER_ID 20
#define CONTROLLED_VAL1_ID 21
#define CONTROLLED_VAL2_ID 22
#define CONTROLLED_VAL3_ID 23
#define CONSOLE_WIDGET_ID 31

config_switch_button_t central_switch_conf{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 3000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};

config_switch_button_t cfg_encoder_clk{
    .debounce_delay_us = 5000,
};

void shared_irq_call_back(uint gpio, uint32_t event_mask);
KY040Encoder encoder = KY040Encoder(ENCODER_ID, ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                    shared_irq_call_back, cfg_encoder_clk);

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

FocusManager focus_manager = FocusManager(FOCUS_MANAGER_ID);
int main()
{
    stdio_init_all();

    SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_ID, CENTRAL_SWITCH_GPIO,
                                               central_switch_conf);

    DisplayEncoderOnTerminal console = DisplayEncoderOnTerminal(CONSOLE_WIDGET_ID);

    ControlledValue val1 = ControlledValue(CONTROLLED_VAL1_ID, -10, +10);
    focus_manager.add_controlled_object(&val1);
    val1.set_current_controller(&encoder);
    val1.set_current_widget(&console);
    ControlledValue val2 = ControlledValue(CONTROLLED_VAL2_ID, 5, 25);
    focus_manager.add_controlled_object(&val2);
    val2.set_current_controller(&encoder);
    val2.set_current_widget(&console);
    ControlledValue val3 = ControlledValue(CONTROLLED_VAL3_ID, -25, -5);
    focus_manager.add_controlled_object(&val3);
    val3.set_current_controller(&encoder);
    val3.set_current_widget(&console);

    UI_ControlledObject *current_cntrl_obj = &focus_manager;
    encoder.set_active_controlled_object(current_cntrl_obj);
    console.set_active_displayed_object(current_cntrl_obj);
    
    while (true)
    {
        if (current_cntrl_obj->has_changed)
        {
            console.display();
            current_cntrl_obj->clear_change_flag();
        }
        ControlEvent sw_event = central_switch.process_sample_event();
        // focus_manager.process_control_event(sw_event);
        switch (sw_event)
        {
        case ControlEvent::PUSH:
            break;
        case ControlEvent::RELEASED_AFTER_SHORT_TIME:
            printf("ID:%d\n", current_cntrl_obj->id);
            if (current_cntrl_obj->id == FOCUS_MANAGER_ID)
            {
                current_cntrl_obj = focus_manager.get_active_controlled_object();
            }
            else
            {
                current_cntrl_obj = &focus_manager;
            }
            printf("ID:%d\n", current_cntrl_obj->id);
            encoder.set_active_controlled_object(current_cntrl_obj);
            console.set_active_displayed_object(current_cntrl_obj);
            console.display();

            break;
        case ControlEvent::LONG_PUSH:
            current_cntrl_obj->reset();
            break;
        case ControlEvent::RELEASED_AFTER_LONG_TIME:
            break;
        default:
            break;
        }
        sleep_ms(20);
    }

    return 0;
}
