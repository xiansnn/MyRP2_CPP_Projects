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
} config_switch_button_t;

class SwitchButton
{
protected:
    uint gpio;
    bool active_lo;
    uint32_t previous_change_time_us;
    /*mechanical switch state machine*/
    bool is_switch_active();
    bool previous_switch_active_state;
    uint debounce_delay_us;
    /*logical button state machine*/
    bool button_is_active;
    uint long_push_delay_us;
    uint long_release_delay_us;

public:
    SwitchButton(uint gpio, config_switch_button_t conf = {});
    ~SwitchButton();
    bool is_button_active();
    SwitchButtonEvent process_sample_event();
};

class SwitchButtonWithIRQ : public SwitchButton
{
protected:
    bool is_switch_pushed(uint32_t current_event_mask);
    uint32_t irq_event_mask_config;

public:
    SwitchButtonWithIRQ(uint gpio, gpio_irq_callback_t call_back, config_switch_button_t conf = {},
                        uint32_t event_mask_config = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE);
    ~SwitchButtonWithIRQ();
    SwitchButtonEvent process_IRQ_event(uint32_t current_event_mask);
    void irq_enabled(bool enabled);
};

#endif // SWITCH_BUTTON_H
