#include "ky_040.h"
#include "ssd1306.h"
#include "widget_bar.h"

#include "probe.h"


#include <string>
#include <vector>

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21
#define SSD1306_I2C_SDA_GPIO 8
#define SSD1306_I2C_SCL_GPIO 9

Probe pr_D1 = Probe(1);

config_master_i2c_t cfg_i2c{
    .i2c = i2c0,
    .sda_pin = SSD1306_I2C_SDA_GPIO,
    .scl_pin = SSD1306_I2C_SCL_GPIO,
    .baud_rate = I2C_FAST_MODE};

config_SSD1306_t cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

config_switch_button_t cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 3000000,
    .long_push_delay_us = 1000000,
    .active_lo = true};

config_switch_button_t cfg_encoder_clk{
    .debounce_delay_us = 5000};

config_bar_widget_t cfg_bar{
    .width = 128,
    .height = 8,
    .with_border = true,
    .with_label = true,
    .font = font_5x8};

void shared_irq_call_back(uint gpio, uint32_t event_mask);

KY040Encoder encoder = KY040Encoder(ENCODER_CLK_GPIO, ENCODER_DT_GPIO, shared_irq_call_back,
                      cfg_encoder_clk);

void shared_irq_call_back(uint gpio, uint32_t event_mask)
{
    switch (gpio)
    {
    case ENCODER_CLK_GPIO:
        encoder.interrupt_service_routine(event_mask);
        break;

    default:
        printf("unknown IRQ\n");
        break;
    }
};

static int current_index = 0;

ControlledValue *next_cntrl_value_index(std::vector<ControlledValue *> values)
{
    current_index++;
    current_index = current_index % values.size();
    return values[current_index];
}

int main()
{
    stdio_init_all();
    hw_I2C_master master = hw_I2C_master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);
    display.clear_pixel_buffer_and_show_full_screen();

    SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_GPIO, cfg_central_switch);

    ControlledValue val0 = ControlledValue(876, 1079, 1, true);
    ControlledValue val1 = ControlledValue(-10, +10);
    ControlledValue val2 = ControlledValue(5, 25);
    ControlledValue val3 = ControlledValue(-25, -5);

    val0.has_changed = true;
    val1.has_changed = true;
    val2.has_changed = true;
    val3.has_changed = true;

    Framebuffer header = Framebuffer(80, 8);
    W_Bar bar1 = W_Bar(&val1, cfg_bar);
    W_Bar bar2 = W_Bar(&val2, cfg_bar);
    W_Bar bar3 = W_Bar(&val3, cfg_bar);

    std::vector<ControlledValue *> cntrl_values = {&val1, &val2, &val3};

    ControlledValue *current_cntrl_value = encoder.set_controlled_object(cntrl_values[0]);

    while (true)
    {
        pr_D1.pulse_us(1);
        if (current_cntrl_value->has_changed)
        {
            current_cntrl_value->clear_change_flag();
            header.clear_text_buffer();
            sprintf(header.text_buffer, "%5.1f MHz", (float)val0.get_value() / 10);
            header.print_text();
            display.show(&header, 40, 0);
            bar1.draw();
            display.show(&bar1, 0, 16);
            bar2.draw();
            display.show(&bar2, 0, 32);
            bar3.draw();
            display.show(&bar3, 0, 48);
        }
        SwitchButtonEvent sw_event = central_switch.process_sample_event();
        switch (sw_event)
        {
        case SwitchButtonEvent::PUSH:
            break;
        case SwitchButtonEvent::RELEASED_AFTER_SHORT_TIME:
            current_cntrl_value = encoder.set_controlled_object(next_cntrl_value_index(cntrl_values));
            break;
        case SwitchButtonEvent::LONG_PUSH:
            break;
        case SwitchButtonEvent::RELEASED_AFTER_LONG_TIME:
            current_cntrl_value->reset_value();
            break;
        default:
            break;
        }
        sleep_ms(20);
    }

    return 0;
}