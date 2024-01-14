// #include "probe.h"
// #include "hw_i2c.h"

// #include "ssd1306.h"
// #include "framebuffer.h"

#include <pico/stdio.h>
#include "hardware/adc.h"
// #include <math.h>
// #include <numbers>
// #include <sstream>
// #include <iomanip>
#include <string>

// #include <stdio.h>
// #include "hardware/gpio.h"
// Probe pr_D4 = Probe(4);
// Probe pr_D5 = Probe(5);
// Probe pr_D6 = Probe(6);
// Probe pr_D7 = Probe(7);

#define KY040_SW 6
#define KY040_CLK 21
#define KY040_DT 26
static const float conversion_factor = 3.3f / (1 << 12);
static const char *gpio_irq_str[] = {
    "LEVEL_LOW",  // 0x1
    "LEVEL_HIGH", // 0x2
    "EDGE_FALL",  // 0x4
    "EDGE_RISE"   // 0x8
};

// config_master_i2c_t cfg_i2c{
//     .i2c = i2c0,
//     .sda_pin = 8,
//     .scl_pin = 9,
//     .baud_rate = I2C_FAST_MODE};

// init_config_SSD1306_t cfg_ssd1306{
//     .i2c_address = 0x3C,
//     .vertical_offset = 0,
//     .scan_SEG_inverse_direction = true,
//     .scan_COM_inverse_direction = true,
//     .contrast = 128,
//     .frequency_divider = 1,
//     .frequency_factor = 0};

static char event_str[128];

void gpio_event_string(char *buf, uint32_t events)
{
    for (uint i = 0; i < 4; i++)
    {
        uint mask = (1 << i);
        if (events & mask)
        {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0')
            {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events)
            {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}
void switch_callback(uint gpio, uint32_t events)
{
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    gpio_event_string(event_str, events);
    printf("GPIO %d %s \n", gpio, event_str);
}
void encoder_callback(uint gpio, uint32_t events)
{
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    int8_t inc{0};
    gpio_event_string(event_str, events);
    printf("GPIO %d %s --> ", gpio, event_str);
    uint16_t result = adc_read();
    if (result > 2000)
        inc = +1;
    else
        inc = -1;

    printf("inc = %+d \n", inc );
}

int main()
{
    stdio_init_all();
    adc_init();
    printf("test KY_040\n");
    printf("ADC Example, measuring GPIO26\n");
    // hw_I2C_master master = hw_I2C_master(cfg_i2c);
    // SSD1306 display = SSD1306(&master, cfg_ssd1306);

    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(KY040_DT);
    // Select ADC input 0 (GPIO26)
    adc_select_input(0);

    gpio_set_irq_enabled_with_callback(KY040_SW, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &switch_callback);
    gpio_set_irq_enabled_with_callback(KY040_CLK, GPIO_IRQ_EDGE_FALL, true, &encoder_callback);
    // gpio_set_irq_enabled_with_callback(KY040_DT, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);


    while (true)
    {
    }

    return 0;
}