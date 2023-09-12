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
    .vertical_offset = 0,
    .scan_SEG_inverse_direction = true,
    .scan_COM_inverse_direction = true,
    .contrast = 128,
    .frequency_divider = 2,
    .frequency_factor = 0};

void test_contrast(SSD1306 *display)
{
    display->clear_screen_buffer_and_GDDRAM();
    render_area_t area = display->compute_render_area(0, 127, 0, 7);
    display->fill_GDDRAM(0x00, area);
    area = display->compute_render_area(48, 80, 3, 4);
    display->fill_GDDRAM(0xFF, area);
    for (size_t i = 0; i < 5; i++)
    {
        display->set_contrast(0);
        sleep_ms(1000);
        display->set_contrast(255);
        sleep_ms(1000);
        display->set_contrast(127);
        sleep_ms(1000);
    }
};

void test_addressing_mode(SSD1306 *display)
{
    uint8_t image[128 * 8]{0x00};
    memset(image, 0xFE, sizeof(image));
    sleep_ms(1000);
    display->clear_screen_buffer_and_GDDRAM();
    render_area_t area;
    // HORIZONTAL_ADDRESSING_MODE
    for (size_t i = 0; i < 4; i++)
    {
        memset(image, 0xAA, sizeof(image));
        area = display->compute_render_area(10 * i, 90 + 10 * i, i, 2 + i);
        display->show_render_area(HORIZONTAL_ADDRESSING_MODE, image, area);
        sleep_ms(1000);
        display->clear_screen_buffer_and_GDDRAM();
    }
    // VERTICAL_ADDRESSING_MODE
    for (size_t i = 0; i < 4; i++)
    {
        memset(image, 0xAA, sizeof(image));
        area = display->compute_render_area(40 + 10 * i, 50 + 10 * i, i, 4 + i);
        display->show_render_area(VERTICAL_ADDRESSING_MODE, image, area);
        sleep_ms(1000);
        display->clear_screen_buffer_and_GDDRAM();
    }
    // PAGE_ADDRESSING_MODE
    for (size_t i = 0; i < 8; i++)
    {
        memset(image, 0x55, sizeof(image));
        area = display->compute_render_area(i * 10, 100 + i * 10, i, i);
        display->show_render_area(PAGE_ADDRESSING_MODE, image, area); // TODO semble dependre de ce qu'il y a avant
        sleep_ms(1000);
    }
};

void test_blink(SSD1306 *display)
{
    render_area_t area;
    display->clear_screen_buffer_and_GDDRAM();
    area = display->compute_render_area(0, 127, 0, 7);
    display->fill_GDDRAM(0x81, area);
    area = display->compute_render_area(64, 96, 2, 5);
    display->fill_GDDRAM(0x7E, area);
    for (int i = 0; i < 3; i++)
    {
        display->send_cmd(SSD1306_SET_DISPLAY_ON); // Set all pixels on
        sleep_ms(500);
        display->send_cmd(SSD1306_SET_RAM_DISPLAY); // go back to following RAM for pixel state
        sleep_ms(500);
    }
    sleep_ms(4500);
};
void test_scrolling(SSD1306 *display)
{
    display->clear_screen_buffer_and_GDDRAM();
    // render 3 cute little raspberries
    render_area_t area = display->compute_render_area(0, IMG_WIDTH - 1, 0, (IMG_HEIGHT / SSD1306_PAGE_HEIGHT) - 1);
    uint8_t offset = 5 + IMG_WIDTH; // 5px padding
    for (int i = 0; i < 3; i++)
    {
        display->show_render_area(HORIZONTAL_ADDRESSING_MODE, raspberry26x32, area);
        area.start_col += offset;
        area.end_col += offset;
    }
    // start scrolling
    config_scroll_t scroll_data = {
        .time_frame_interval = _25_FRAMES,
        .vertical_scrolling_offset = 1};
    display->horizontal_scroll(true, scroll_data);
    sleep_ms(5000);
    display->horizontal_scroll(false, scroll_data);
    sleep_ms(1000);
    display->vertical_scroll(true, scroll_data);
    sleep_ms(5000);
    display->vertical_scroll(false, scroll_data);
};
void test_drawline(SSD1306 *display)
{
    render_area_t full_screen_area = display->compute_render_area(0,127,0,7);
    bool pix = true;
    for (int i = 0; i < 2; i++)
    {
        for (int x = 0; x < SSD1306_WIDTH; x++)
        {
            display->DrawLine(display->screen_buffer, x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, pix);
            display->show_render_area(HORIZONTAL_ADDRESSING_MODE, display->screen_buffer, full_screen_area);
        }

        for (int y = SSD1306_HEIGHT - 1; y >= 0; y--)
        {
            display->DrawLine(display->screen_buffer, 0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, pix);
            display->show_render_area(HORIZONTAL_ADDRESSING_MODE, display->screen_buffer, full_screen_area);
        }
        pix = false;
    }
};

int main()
{
    stdio_init_all();
    // create I2C bus hw peripheral and display
    hw_I2C_master master = hw_I2C_master(i2c_cfg);
    SSD1306 display = SSD1306(&master, ssd1306_cfg);

    // printf("hello from SSD1306\n");

    while (true)
    {
        // test_blink(&display);
        // test_contrast(&display);
        // test_addressing_mode(&display);
        // test_scrolling(&display);
        test_drawline(&display);
    }

    return 0;
}
