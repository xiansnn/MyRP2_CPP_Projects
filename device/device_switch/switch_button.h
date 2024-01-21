#if !defined(SWITCH_BUTTON_H)
#define SWITCH_BUTTON_H

#include "pico/stdio.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define DEBOUNCE_us 10000
#define LONG_PRESS_DELAY_us 1000000
#define GPIO_HI true
#define GPIO_LO false

enum class SwitchButtonStatus {
    PRESSED,
    RELEASED,
};

enum class SwitchButtonEvent {
    NOOP,
    PUSH,
    LONG_PUSH,
    RELEASE
};

typedef struct switch_button_config
{
    uint debounce_delay_us = DEBOUNCE_us;
    uint long_press_delay_us = LONG_PRESS_DELAY_us;
    bool active_lo = true;
} switch_button_config_t;

class SwitchButton
{
private:
    uint gpio;
    uint64_t previous_change_time_us;
    bool previous_push_state;
    SwitchButtonStatus previous_button_status;
    SwitchButtonStatus current_status;

    uint debounce_delay_us;
    uint long_press_delay_us;
    bool active_lo;

public:
    SwitchButton(uint gpio, switch_button_config_t conf={});
    ~SwitchButton();
    SwitchButtonEvent get_event();
    SwitchButtonStatus get_status();
    bool check_switch_pushed();
};

#endif // SWITCH_BUTTON_H
