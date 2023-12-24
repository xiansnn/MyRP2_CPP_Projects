#include "probe.h"
// #include <stdlib.h>
#include "hw_i2c.h"
#include <pico/stdio.h>
#include "ssd1306.h"
#include "framebuffer.h"


// #include <string.h>
// #include <string>
// #include <math.h>
// #include <numbers>
#include <sstream>
#include <iomanip>
// #include <vector>

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

void test_text_stream(SSD1306 *display)
{
    pr_D4.hi();
    display->clear_buffer_and_show_GDDRAM();
    pr_D4.lo(); // 25 ms

    const unsigned char *current_font{font_8x8};
    // uint8_t w = current_font[FONT_WIDTH];
    uint8_t h = current_font[FONT_HEIGHT];
    // uint8_t nb_text_line = SSD1306_HEIGHT / h;

    std::ostringstream stream0, stream1, stream2, stream3 ;

    stream0 << "test";
    
    int n = 10;
    stream1 << n; 

    float f = 3.1415926;
    stream2 << f;

    stream3 << std::endl;

    // std::string[] lines={""}; // faire un ttableau de 4 ou 5 string avec des formatages differents

    printf("stream0:%10s |stream1: %10s |stream2: %10s |stream3: %10s \n", 
                                                    stream0.str().c_str(), 
                                                    stream1.str().c_str(), 
                                                    stream2.str().c_str(), 
                                                    stream3.str().c_str());
    display->text(current_font, stream0.str().c_str(), 0, 0);
    display->text(current_font, stream1.str().c_str(), 0, h);
    display->text(current_font, stream2.str().c_str(), 0, 2*h);
    display->text(current_font, stream3.str().c_str(), 0, 3*h);
    display->show();

    sleep_ms(1000);
}

int main()
{
    stdio_init_all();
    // create I2C bus hw peripheral and display
    hw_I2C_master master = hw_I2C_master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    while (true)
    {
        test_text_stream(&display);
    }

    return 0;
}
