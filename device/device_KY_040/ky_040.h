#if !defined(KY_040_H)
#define KY_040_H

#include <pico/stdio.h>
#include "switch_button.h"
#include "controlled_value.h"

class KY040_IRQ : public SwitchButtonWithIRQ
{
private:
    uint dt_gpio;
    ControlledValue* cntrl_value;

public:
    KY040_IRQ(uint encoder_clk_gpio, uint encoder_dt_gpio,
              gpio_irq_callback_t call_back, switch_button_config_t clk_conf = {},ControlledValue* ctrl_value = nullptr
              );
    ~KY040_IRQ();
    void process_encoder_event();
    void add_cntrl_value(ControlledValue* val);
    ControlledValue* get_current_cntrl_value();
};

#endif // KY_040_H
