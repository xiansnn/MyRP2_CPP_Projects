#include "ky_040.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string>

KY040Encoder::KY040Encoder(uint8_t id, uint encoder_clk_gpio, uint encoder_dt_gpio, gpio_irq_callback_t call_back,
                           config_switch_button_t clk_conf)
    : SwitchButtonWithIRQ(id, encoder_clk_gpio, call_back, clk_conf) // TODO change name : RotaryEncoder
{
    this->dt_gpio = encoder_dt_gpio;
    this->active_lo = true;

    gpio_init(this->dt_gpio);
    gpio_pull_up(this->dt_gpio);
}

KY040Encoder::~KY040Encoder()
{
}

void KY040Encoder::interrupt_service_routine(uint32_t current_irq_event_mask)
{
    irq_enabled(false);
    ControlEvent clk_event = process_IRQ_event(current_irq_event_mask);
    if (clk_event == ControlEvent::PUSH)
    {
        bool clockwise_rotation = gpio_get(dt_gpio);
        if (clockwise_rotation)
            this->active_controlled_object->process_control_event(ControlEvent::INCREMENT);
        else
            this->active_controlled_object->process_control_event(ControlEvent::DECREMENT);
    }
    irq_enabled(true);
}
