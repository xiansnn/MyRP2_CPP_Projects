#if !defined(ROTARY_ENCODER_H)
#define ROTARY_ENCODER_H

#include <pico/stdio.h>
#include "switch_button.h"
#include "controlled_value.h"
#include "ui_mvc.h"

class RotaryEncoder : public SwitchButtonWithIRQ
{
private:
    uint dt_gpio;

public:
    RotaryEncoder(uint8_t id, uint encoder_clk_gpio, uint encoder_dt_gpio,
              gpio_irq_callback_t call_back, config_switch_button_t clk_conf = {});
    ~RotaryEncoder();
    void interrupt_service_routine(uint32_t irq_event_mask);

};

#endif // ROTARY_ENCODER_H
