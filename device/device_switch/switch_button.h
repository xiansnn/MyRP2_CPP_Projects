#if !defined(SWITCH_BUTTON_H)
#define SWITCH_BUTTON_H

#include "pico/stdio.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define GPIO_HI true
#define GPIO_LO false

enum class ButtonState
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

#define DEBOUNCE_us 10000
#define LONG_RELEASE_DELAY_us 3000000
#define LONG_PUSH_DELAY_us 1000000

typedef struct switch_button_config
{
    uint debounce_delay_us = DEBOUNCE_us;
    uint long_release_delay_us = LONG_RELEASE_DELAY_us;
    uint long_push_delay_us = LONG_PUSH_DELAY_us;
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
    ButtonState button_state;
    uint long_push_delay_us;
    uint long_release_delay_us;

public:
    SwitchButton(uint gpio, switch_button_config_t conf = {});
    ~SwitchButton();
    SwitchButtonEvent get_sample_event();
    ButtonState get_button_logical_state();
    bool is_switch_active();
};

class SwitchButtonWithIRQ : public SwitchButton
{
private:
    gpio_irq_callback_t _call_back;
    uint32_t _sw_event_mask;
public:
    SwitchButtonWithIRQ(uint gpio, gpio_irq_callback_t call_back, switch_button_config_t conf = {}, 
    uint32_t sw_event_mask = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE);
    ~SwitchButtonWithIRQ();
    SwitchButtonEvent get_IRQ_event();
};

#endif // SWITCH_BUTTON_H
