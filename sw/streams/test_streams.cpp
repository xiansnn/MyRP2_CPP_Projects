#include "probe.h"
// #include <stdlib.h>
#include "hw_i2c.h"
#include <pico/stdio.h>
#include "ssd1306.h"
#include "framebuffer.h"

// #include <string.h>
#include <math.h>
#include <numbers>
#include <sstream>
#include <iomanip>
// #include "../../../../../../Program Files/Raspberry Pi/Pico SDK v1.5.1/gcc-arm-none-eabi/arm-none-eabi/include/c++/10.3.1/bits/ios_base.h"
// #include <vector>
#include <string>

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

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

void test_string_and_stream_format(SSD1306 *display)
{
    pr_D4.hi();
    display->clear_buffer_and_show_full_screen();
    pr_D4.lo(); // 25.74 ms

    const unsigned char *current_font{font_5x8};
    uint8_t h = current_font[FONT_HEIGHT];

    int n = 42;
    float f = std::numbers::pi;

    std::ostringstream stream0, stream1, stream2;

    stream0.fill('.');
    stream2.fill('.');
    stream2.precision(4);
    stream2.width(20);

    pr_D5.hi(); // 1.5 ms since  pr_D4.lo();
    stream0 << std::left << std::setw(6) << "test";
    display->text(current_font, stream0.str(), 0, 0);
    stream1 << std::setw(5) << std::dec << n << "|" << std::setw(5) << std::showbase << std::hex << n << "|" << std::showbase << std::setw(5) << std::oct << n;
    display->text(current_font, stream1.str(), 0, 1 * h);
    stream2 << "PI = " << std::left << f;
    display->text(current_font, stream2.str(), 0, 2 * h);
    pr_D5.lo(); // 1.246 ms

    char buf[20];

    pr_D6.hi();
    sprintf(buf, "%-20s", "test");
    display->text(current_font, buf, 0, 4 * h);
    sprintf(buf, "%5d|%#5x|%#5o", n, n, n);
    display->text(current_font, buf, 0, 5 * h);
    sprintf(buf, "PI = %12.3f", f);
    display->text(current_font, buf, 0, 6 * h);
    pr_D6.lo(); // 0.581 ms

    pr_D7.hi();
    display->show();
    pr_D7.lo(); // 25.77 ms

    sleep_ms(1000);
}
void test_sprintf_format(SSD1306 *display)
{
    display->clear_buffer_and_show_full_screen();
    const unsigned char *current_font{font_5x8};

    uint8_t h = current_font[FONT_HEIGHT];
    uint8_t char_per_line = SSD1306_WIDTH / current_font[FONT_WIDTH];

    const char *s = "Hello";
    // char buf[25]; // nb of char per line is 25 with 5x8 font
    char *c_str = new char[char_per_line + 1];

    sprintf(c_str, "Strings:");
    display->text(current_font, c_str, 0, 0 * h);
    sprintf(c_str, " padding:");
    display->text(current_font, c_str, 0, 1 * h);
    sprintf(c_str, "  [%10s]", s);
    display->text(current_font, c_str, 0, 2 * h);
    sprintf(c_str, "  [%-10s]", s);
    display->text(current_font, c_str, 0, 3 * h);
    sprintf(c_str, "  [%*s]", 10, s);
    display->text(current_font, c_str, 0, 4 * h);
    sprintf(c_str, " truncating:");
    display->text(current_font, c_str, 0, 5 * h);
    sprintf(c_str, "  %.4s", s);
    display->text(current_font, c_str, 0, 6 * h);
    sprintf(c_str, "  %.*s\n", 3, s);
    display->text(current_font, c_str, 0, 7 * h);

    display->show();
    sleep_ms(2000);
    display->clear_buffer();

    printf("Characters:\t%c %%\n", 'A');
    sprintf(c_str, "Characters:  %c %%", 'A');
    display->text(current_font, c_str, 0, 0 * h);

    display->show();
    sleep_ms(2000);
    display->clear_buffer();

    sprintf(c_str, "Integers:");
    display->text(current_font, c_str, 0, 0 * h);
    sprintf(c_str, "  Dec:  %i %d %.3i %i %.0i %+i %i", 1, 2, 3, 0, 0, 4, -4);
    display->text(current_font, c_str, 0, 1 * h);
    sprintf(c_str, "  Hex:  %x %x %X %#x", 5, 10, 10, 6);
    display->text(current_font, c_str, 0, 2 * h);
    sprintf(c_str, "  Oct:    %o %#o %#o", 10, 10, 4);
    display->text(current_font, c_str, 0, 3 * h);
    sprintf(c_str, "Floating point:");
    display->text(current_font, c_str, 0, 4 * h);
    sprintf(c_str, "  Rnd:  %f %.0f %.3f", 1.5, 1.5, 1.5);
    display->text(current_font, c_str, 0, 5 * h);
    sprintf(c_str, "  Pad:  %05.2f %.2f %5.2f", 1.5, 1.5, 1.5);
    display->text(current_font, c_str, 0, 6 * h);
    sprintf(c_str, "  Sci:  %.3E %.1e", 1.5, 1.5);
    display->text(current_font, c_str, 0, 7 * h);

    display->show();
    sleep_ms(2000);
    display->clear_buffer();

    delete[] c_str;

    //----------------------------------------
    // undefined result for this compiler
    // printf("\tHexadecimal:\t%a %A\n", 1.5, 1.5);
    // printf("\tSpecial values:\t0/0=%g 1/0=%g\n", 0.0 / 0.0, 1.0 / 0.0);
    // printf("Fixed-width types:\n");
    // printf("\tLargest 32-bit value is %" PRIu32 " or %#" PRIx32 "\n",
    //        UINT32_MAX, UINT32_MAX);
    //----------------------------------------
}

void test_string_and_framebuffer(SSD1306 *display)
{
}

int main()
{
    stdio_init_all();
    hw_I2C_master master = hw_I2C_master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    while (true)
    {
        test_sprintf_format(&display);
        // test_string_and_stream_format(&display);
        // test_string_and_framebuffer(&display);
    }

    // return 0;
}
