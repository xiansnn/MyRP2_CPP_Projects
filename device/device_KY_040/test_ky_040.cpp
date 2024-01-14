#include "probe.h"
#include "ky_040.h"

#include <string>

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

#define KY040_SW 6
#define KY040_CLK 21
#define KY040_DT 26
static const char *gpio_irq_str[] = {
    "LEVEL_LOW",  // 0x1
    "LEVEL_HIGH", // 0x2
    "EDGE_FALL",  // 0x4
    "EDGE_RISE"   // 0x8
};
static char event_str[128];

static int value{0};

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
    gpio_event_string(event_str, events);
    switch (gpio)
    {
    case KY040_SW:
        printf("Switch(gpio:%d)  %s\n", gpio, event_str);
        break;
    case KY040_CLK:
        if (adc_read() > 2000)
            value++;
        else
            value--;

        printf("value = %+d \n", value);
        break;
    default:
        break;
    }
    pr_D5.lo();
}

int main()
{
    stdio_init_all();
    printf("test KY_040\n");

    KY040 encoder = KY040(KY040_SW, KY040_CLK, KY040_DT, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, &encoder_callback);

    while (true)
    {
    }
}