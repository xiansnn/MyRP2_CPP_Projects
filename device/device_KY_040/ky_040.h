#if !defined(KY_040_H)
#define KY_040_H

#include <pico/stdio.h>
#include "hardware/adc.h"

class KY040
{
private:
    /* data */
public:
    KY040(uint switch_gpio, uint encoder_clk, uint encoder_dt, uint32_t sw_event_mask, gpio_irq_callback_t call_back);
    ~KY040();
};





#endif // KY_040_H
