#include "ssd1306.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raspberry26x32.h"
#include "ssd1306_font.h"

config_master_i2c_t i2c_cfg{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE};

init_config_SSD1306_t ssd1306_cfg{
    .i2c_address = 0x3C,
    .mux_ratio_value = 64,
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .sequential_COM = true,
    .enable_COM_L_R_remap = true,
    .contrast = 255,
    .frequency_divider = 1,
    .frequency_factor = 8};

void test_blink(SSD1306 display)
{
    uint8_t buf[SSD1306_BUF_LEN];
    memset(buf, 0, SSD1306_BUF_LEN);
    display.render(buf);
    // intro sequence: flash the screen 3 times
    for (int i = 0; i < 3; i++)
    {
        display.send_cmd(SSD1306_SET_DISPLAY_ON); // Set all pixels on
        sleep_ms(500);
        display.send_cmd(SSD1306_SET_RAM_DISPLAY); // go back to following RAM for pixel state
        sleep_ms(500);
    }
};
void test_raspberry(SSD1306 display)
{
    // render 3 cute little raspberries
    // struct render_area area = {
    //     start_page : 0,
    //     end_page : (IMG_HEIGHT / SSD1306_PAGE_HEIGHT)  - 1
    // };
    display.area.start_page = 0;
    display.area.end_page = (IMG_HEIGHT / SSD1306_PAGE_HEIGHT) - 1;
    display.area.start_col = 0;
    display.area.end_col = IMG_WIDTH - 1;
    display.calc_render_area_buflen();

    uint8_t offset = 5 + IMG_WIDTH; // 5px padding

    for (int i = 0; i < 3; i++)
    {
        display.render(raspberry26x32);
        display.area.start_col += offset;
        display.area.end_col += offset;
    }

    display.scroll(true);
    sleep_ms(5000);
    display.scroll(false);
};

int main()
{
    stdio_init_all();
    // create I2C bus hw peripheral and display
    hw_I2C_master master = hw_I2C_master(i2c_cfg);
    SSD1306 display = SSD1306(&master, ssd1306_cfg);

    printf("hello from SSD1306\n");

    while (true)
    {
        test_blink(display);
        test_raspberry(display);
    }

    return 0;
}
