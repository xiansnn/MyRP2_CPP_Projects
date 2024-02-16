#if !defined(KY_040_H)
#define KY_040_H

#include <pico/stdio.h>
#include "switch_button.h"
#include "controlled_value.h"
#include "ui_mvc.h"

class KY040Encoder : public SwitchButtonWithIRQ , public UI_Controller
{
private:
    uint dt_gpio;
    ControlledValue* cntrl_value;

public:
    KY040Encoder(uint encoder_clk_gpio, uint encoder_dt_gpio,
              gpio_irq_callback_t call_back, config_switch_button_t clk_conf = {},ControlledValue* ctrl_value = nullptr
              );
    ~KY040Encoder();
    void interrupt_service_routine(uint32_t irq_event_mask);
    ControlledValue* set_cntrl_value(ControlledValue* val);
    ControlledValue* get_cntrl_value();
};

#endif // KY_040_H
