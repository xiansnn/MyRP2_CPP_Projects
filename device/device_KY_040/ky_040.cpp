#include "ky_040.h"

KY040::KY040(uint switch_gpio, uint encoder_clk, uint encoder_dt, uint32_t sw_event_mask, gpio_irq_callback_t call_back)
{
    adc_init();
    adc_gpio_init(encoder_dt);
    adc_select_input(encoder_dt - 26); // Select ADC input (GPIO26 <=> channel 0)

    gpio_set_irq_enabled_with_callback(encoder_clk, GPIO_IRQ_EDGE_FALL, true, call_back);

    gpio_set_irq_enabled_with_callback(switch_gpio, sw_event_mask, true, call_back);

    // gpio_set_input_enabled(KY040_CLK, true);
    // gpio_set_irq_enabled(KY040_CLK, GPIO_IRQ_EDGE_FALL, true);
    // gpio_set_input_enabled(KY040_SW, true);
    // gpio_set_irq_enabled(switch_gpio, GPIO_IRQ_EDGE_FALL , true);
    // gpio_set_irq_callback(call_back);
}

KY040::~KY040()
{
}
