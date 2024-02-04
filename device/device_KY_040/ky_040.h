#if !defined(KY_040_H)
#define KY_040_H

#include <pico/stdio.h>
#include "switch_button.h"

enum class EncoderEvent
{
    NOOP,
    INCREMENT,
    DECREMENT
};

class KY040_IRQ : public SwitchButton
{
private:
    uint dt_gpio;
    public:
    KY040_IRQ(uint encoder_clk_gpio, uint encoder_dt_gpio,
              gpio_irq_callback_t call_back, 
              switch_button_config_t clk_conf = {});
    ~KY040_IRQ();
    EncoderEvent get_encoder_event();
    };

#endif // KY_040_H
