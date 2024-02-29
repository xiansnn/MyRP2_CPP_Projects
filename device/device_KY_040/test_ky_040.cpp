#include "probe.h"
#include "rotary_encoder.h"

#include <string>
#include <vector>

#include "print_console_line.h"
#include "KY_040_focus_manager.h"

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21

#define ENCODER_ID 11
#define CENTRAL_SWITCH_ID 12

#define CONTROLLED_VAL1_ID 21
#define CONTROLLED_VAL2_ID 22
#define CONTROLLED_VAL3_ID 23

#define CONSOLE_WIDGET_ID0 30
#define CONSOLE_WIDGET_ID1 31
#define CONSOLE_WIDGET_ID2 32
#define CONSOLE_WIDGET_ID3 33

config_switch_button_t central_switch_conf{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};

config_switch_button_t cfg_encoder_clk{
    .debounce_delay_us = 5000};

void shared_irq_call_back(uint gpio, uint32_t event_mask);
RotaryEncoder encoder = RotaryEncoder(ENCODER_ID, ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                      shared_irq_call_back, cfg_encoder_clk);

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

KY_40_WidgetManager widget_manager = KY_40_WidgetManager();

// W_DisplayControlledValueOnTerminal console = W_DisplayControlledValueOnTerminal(CONSOLE_WIDGET_ID0, 0, 0);
W_DisplayControlledValueOnTerminal console1 = W_DisplayControlledValueOnTerminal(CONSOLE_WIDGET_ID1, 0, 0);
W_DisplayControlledValueOnTerminal console2 = W_DisplayControlledValueOnTerminal(CONSOLE_WIDGET_ID2, 0, 0);
W_DisplayControlledValueOnTerminal console3 = W_DisplayControlledValueOnTerminal(CONSOLE_WIDGET_ID3, 0, 0);

ControlledValue val1 = ControlledValue(CONTROLLED_VAL1_ID, -10, +10);
ControlledValue val2 = ControlledValue(CONTROLLED_VAL2_ID, 5, 25);
ControlledValue val3 = ControlledValue(CONTROLLED_VAL3_ID, -25, -5);

int main()
{
    stdio_init_all();

    encoder.set_active_controlled_object(&widget_manager);

    widget_manager.add_controlled_object(&val1);
    widget_manager.add_controlled_object(&val2);
    widget_manager.add_controlled_object(&val3);

    // console.set_active_displayed_object(&widget_manager);
    console1.set_active_displayed_object(&val1);
    console2.set_active_displayed_object(&val2);
    console3.set_active_displayed_object(&val3);

    // UI_ControlledObject *current_cntrl_obj = &focus_manager;
    // widget_manager.add_widget(&console);
    widget_manager.add_widget(&console1);
    widget_manager.add_widget(&console2);
    widget_manager.add_widget(&console3);

    // console.set_active_displayed_object(widget_manager.get_active_controlled_object());

    encoder.set_active_controlled_object(widget_manager.get_active_controlled_object());

    while (true)
    {
        widget_manager.process_control_event(&central_switch);
        widget_manager.refresh();
        if (widget_manager.active_controlled_object_has_changed) // TODO voir comment supprimer active_controlled_object_has_changed
        {
            encoder.set_active_controlled_object(widget_manager.get_active_controlled_object());
            widget_manager.clear_active_controlled_object_change_flag();
        }

        sleep_ms(20);
    }

    return 0;
}
