#if !defined(SWITCH_BUTTON_H)
#define SWITCH_BUTTON_H

#include "pico/stdio.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define DEBOUNCE_us 10000
#define LONG_PRESS_DELAY_us 1000000
#define GPIO_HI true
#define GPIO_LO false

enum class ButtonStatus
{
    ACTIVE,
    INACTIVE,
};

enum class SwitchButtonEvent
{
    NOOP,
    PUSH,
    LONG_PUSH,
    RELEASED_AFTER_LONG_TIME,
    RELEASED_AFTER_SHORT_TIME
};

typedef struct switch_button_config
{
    uint debounce_delay_us = DEBOUNCE_us;
    uint long_press_delay_us = LONG_PRESS_DELAY_us;
    bool active_lo = true;
} switch_button_config_t;

class SwitchButton
{
protected:
    /*mechanical switch state machine*/
    uint gpio;
    bool active_lo;
    bool previous_switch_active_state;
    uint64_t previous_change_time_us;
    uint debounce_delay_us;
    /*logical button state machine*/
    ButtonStatus previous_button_state;
    ButtonStatus current_button_state;
    uint long_press_delay_us;

public:
    SwitchButton(uint gpio, switch_button_config_t conf = {});
    ~SwitchButton();
    SwitchButtonEvent get_event();
    ButtonStatus get_status();
    bool get_switch_activation_state();
};

class SwitchButtonWithIRQ : public SwitchButton
{
private:
    /* data */
public:
    SwitchButtonWithIRQ(uint gpio, gpio_irq_callback_t call_back, switch_button_config_t conf = {}, uint32_t sw_event_mask = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE);
    ~SwitchButtonWithIRQ();
};

#endif // SWITCH_BUTTON_H
