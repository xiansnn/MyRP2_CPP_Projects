#include <hw_i2c.h>
#include "ssd1306.h"
#include "ky_040.h"
#include "probe.h"
#include <string>

#define KY040_SW 6
#define KY040_CLK 21
#define KY040_DT 26

config_master_i2c_t cfg_i2c{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE};

init_config_SSD1306_t cfg_ssd1306{
    .i2c_address = 0x3C,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 1,
    .frequency_factor = 0};

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

static const char *gpio_irq_str[] = {
    "LEVEL_LOW",  // 0x1
    "LEVEL_HIGH", // 0x2
    "EDGE_FALL",  // 0x4
    "EDGE_RISE"   // 0x8
};
static char event_str[128];

static int8_t value{0};
static uint32_t prec = time_us_32();
static uint32_t debounce = 100000;

void gpio_event_string(char *buf, uint32_t events)
{
    pr_D4.hi();
    for (uint i = 0; i < 4; i++)
    {
        uint mask = (1 << i);
        if (events & mask)
        {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0')
                *buf++ = *event_str++;

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
    pr_D4.lo();
}

void encoder_callback(uint gpio, uint32_t events)
{
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    pr_D5.hi();
    bool dt;
    uint32_t current_time, delta_time;
    gpio_event_string(event_str, events);
    switch (gpio)
    {
    case KY040_SW:
        printf("Switch(gpio:%d)  %s\n", gpio, event_str);
        value = 0;
        break;
    case KY040_CLK:
        current_time = time_us_32();
        delta_time = current_time- prec;

        if (delta_time > debounce)
        {
            dt = gpio_get(KY040_DT);
            printf("dt: %x  delta: %f ", dt, (float)delta_time/1000.0);
            if (!dt)
                value++; // = std::min(100, value+1);
            else
                value--; // = std::max (100, value-1);
            value = value > 10 ? 10 : value;
            value = value < -10 ? -10 : value;
            printf("value = %+d \n", value);
        }
        prec = current_time;
        break;
    default:
        break;
    }
    pr_D5.lo();
}

int main()
{

    stdio_init_all();
    hw_I2C_master master = hw_I2C_master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);
    KY040 encoder = KY040(KY040_SW, KY040_CLK, KY040_DT, GPIO_IRQ_EDGE_FALL, &encoder_callback);

    display.clear_pixel_buffer_and_show_full_screen();
    const unsigned char *current_font = font_12x16;

    uint8_t w = current_font[FONT_WIDTH];
    uint8_t h = current_font[FONT_HEIGHT];

    /* draw title*/
    int title_width = w * 6;
    int title_height = h;
    Framebuffer fb_title = Framebuffer(title_width, title_height, Framebuffer_format::MONO_VLSB);
    fb_title.set_font(current_font);
    fb_title.print_text("value:");
    display.show(&fb_title, 0, 0);

    // draw value
    int value_width = w * 4;
    int value_height = h;
    Framebuffer fb_value = Framebuffer(value_width, value_height, Framebuffer_format::MONO_VLSB);
    fb_value.set_font(current_font);

    // draw graph
    int graph_height = 5;
    int graph_width = 128;
    Framebuffer fb_graph_1 = Framebuffer(graph_width, graph_height, Framebuffer_format::MONO_VLSB);
    Framebuffer fb_graph_2 = Framebuffer(graph_width, graph_height, Framebuffer_format::MONO_VLSB);

    while (true)
    {
        sprintf(fb_value.text_buffer, "%+4d", value);
        fb_value.print_text();
        fb_graph_1.rect(0, 0, graph_width, graph_height, false);
        fb_graph_2.rect(0, 0, graph_width, graph_height, false);
        if (value >= 0)
        {
            fb_graph_1.rect(0, 0, value * 10, graph_height, true);
        }
        else
        {
            fb_graph_2.rect(0, 0, value * -10, graph_height, true);
        }

        display.show(&fb_value, title_width, 0);
        int anchor_y_graph = 20;
        display.show(&fb_graph_1, 0, anchor_y_graph);
        display.show(&fb_graph_2, 0, anchor_y_graph + graph_height - 1);
        sleep_ms(10);
        fb_graph_1.clear_pixel_buffer();
        fb_graph_2.clear_pixel_buffer();
    }
    return 0;
}
