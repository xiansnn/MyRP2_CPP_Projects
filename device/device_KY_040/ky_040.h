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

class KY040 : public SwitchButton
{
private:
    uint dt_gpio;
public:
    KY040(uint encoder_clk_gpio, uint encoder_dt_gpio,
          switch_button_config_t clk_conf = {});
    ~KY040();
    EncoderEvent get_event();
};

class KY040_IRQ : public KY040
{
private:
public:
    KY040_IRQ(uint encoder_clk_gpio, uint encoder_dt_gpio,
              gpio_irq_callback_t call_back,
              switch_button_config_t clk_conf = {}, uint32_t clk_event_mask = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE);
    ~KY040_IRQ();
};

#endif // KY_040_H
