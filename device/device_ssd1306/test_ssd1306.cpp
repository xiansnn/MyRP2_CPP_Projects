#include "ssd1306.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "raspberry26x32.h"
#include <math.h>
#include <numbers>
#include "probe.h"
#include <sstream>
#include <iomanip>

#define DEGREE "\xF8"

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

void test_contrast(SSD1306 *display)
{
    display->clear_buffer_and_show_full_screen();
    render_area_t area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    display->fill_pattern_and_show_GDDRAM(0x55, area);
    area = SSD1306::compute_render_area(32, 96, 16, 32);
    display->fill_pattern_and_show_GDDRAM(0xFF, area);
    for (size_t i = 0; i < 3; i++)
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
    display->clear_buffer_and_show_full_screen();
    render_area_t area;
    // HORIZONTAL_ADDRESSING_MODE
    for (size_t i = 0; i < 4; i++)
    {
        memset(image, 0xAA, sizeof(image));
        area = SSD1306::compute_render_area(10 * i, 90 + 10 * i, 8 * i, 2 + 8 * i);
        display->show_render_area(image, area, HORIZONTAL_ADDRESSING_MODE);
        sleep_ms(1000);
        display->clear_buffer_and_show_full_screen();
    }
    // VERTICAL_ADDRESSING_MODE
    for (size_t i = 0; i < 4; i++)
    {
        memset(image, 0xAA, sizeof(image));
        area = SSD1306::compute_render_area(40 + 10 * i, 50 + 10 * i, 8 * i, 30 + 8 * i);
        display->show_render_area(image, area, VERTICAL_ADDRESSING_MODE);
        sleep_ms(1000);
        display->clear_buffer_and_show_full_screen();
    }
    // PAGE_ADDRESSING_MODE
    for (size_t i = 0; i < 8; i++)
    {
        memset(image, 0x55, sizeof(image));
        area = SSD1306::compute_render_area(i * 10, 100 + i * 10, 8 * i, 8 * i);
        display->show_render_area(image, area, PAGE_ADDRESSING_MODE); // TODO semble dependre de ce qu'il y a avant
        sleep_ms(1000);
    }
};

void test_blink(SSD1306 *display)
{
    render_area_t area;
    display->clear_buffer_and_show_full_screen();
    area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    display->fill_pattern_and_show_GDDRAM(0x81, area);
    area = SSD1306::compute_render_area(64, 96, 15, 40);
    display->fill_pattern_and_show_GDDRAM(0x7E, area);
    for (int i = 0; i < 2; i++)
    {
        display->set_all_pixel_ON();
        sleep_ms(1000);
        display->set_display_from_RAM();
        sleep_ms(1000);
    }
};
void test_scrolling(SSD1306 *display)
{
    display->clear_buffer_and_show_full_screen();
    // render 3 cute little raspberries
    render_area_t area = SSD1306::compute_render_area(0, IMG_WIDTH - 1, 0, IMG_HEIGHT - 1);
    uint8_t offset = 5 + IMG_WIDTH; // 5px padding
    for (int i = 0; i < 3; i++)
    {
        display->show_render_area(raspberry26x32, area);
        area.start_col += offset;
        area.end_col += offset;
    }
    // start scrolling
    config_scroll_t scroll_data = {
        .time_frame_interval = _25_FRAMES,
        .vertical_scrolling_offset = 1};
    display->horizontal_scroll(true, scroll_data);
    sleep_ms(3000);
    display->horizontal_scroll(false, scroll_data);
    sleep_ms(1000);
    display->vertical_scroll(true, scroll_data);
    sleep_ms(5000);
    display->vertical_scroll(false, scroll_data);
};
void test_outofframe_line(SSD1306 *display)
{
    int y0, x1, y1;
    display->clear_buffer_and_show_full_screen();
    x1 = 64;
    y1 = 70;
    y0 = -10;

    for (int x = -10; x < 138; x++)
    {
        Framebuffer_color c = Framebuffer_color::white;
        display->line(x, y0, x1, y1, c);
        display->show();
        c = Framebuffer_color::black;
        display->line(x, y0, x1, y1, c);
        display->show();
    }
};
void test_fb_line(SSD1306 *display)
{
    display->clear_buffer_and_show_full_screen();
    Framebuffer_color c = Framebuffer_color::black;
    render_area_t full_screen_area = SSD1306::compute_render_area(0, SSD1306_WIDTH - 1, 0, SSD1306_HEIGHT - 1);
    for (int i = 0; i < 2; i++)
    {
        if (c == Framebuffer_color::black)
            c = Framebuffer_color::white;
        else
            c = Framebuffer_color::black;

        for (int x = 0; x < SSD1306_WIDTH; x++)
        {
            display->line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            display->show();
        }

        for (int y = SSD1306_HEIGHT - 1; y >= 0; y--)
        {
            display->line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            display->show();
        }
    }

    sleep_ms(1000);
    for (int i = 0; i < 2; i++)
    {
        for (int x = 0; x < SSD1306_WIDTH; x++)
        {
            c = Framebuffer_color::white;
            display->line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            display->show();
            c = Framebuffer_color::black;
            display->line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
            display->show();
        }

        for (int y = SSD1306_HEIGHT - 1; y >= 0; y--)
        {
            c = Framebuffer_color::white;
            display->line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            display->show_render_area(display->buffer, full_screen_area);
            c = Framebuffer_color::black;
            display->line(0, y, SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1 - y, c);
            display->show_render_area(display->buffer, full_screen_area);
        }
    }
    sleep_ms(1000);
};

void test_fb_hline(SSD1306 *display)
{
    display->clear_buffer_and_show_full_screen();
    display->hline(0, 0, 32); //, Framebuffer_color::white);
    display->show();
    sleep_ms(1000);
    display->hline(0, 15, 64); //, Framebuffer_color::white);
    display->show();
    sleep_ms(1000);
    display->hline(0, 31, 96); //, Framebuffer_color::white);
    display->show();
    sleep_ms(1000);
    display->hline(0, 47, 128); //, Framebuffer_color::white);
    display->hline(0, 63, 128); //, Framebuffer_color::white);
    display->show();
    sleep_ms(1000);
}
void test_fb_vline(SSD1306 *display)
{
    display->clear_buffer_and_show_full_screen();
    display->vline(0, 0, 16); //, Framebuffer_color::white);
    display->show();
    sleep_ms(1000);
    display->vline(15, 0, 32); //, Framebuffer_color::white);
    display->show();
    sleep_ms(1000);
    display->vline(31, 0, 48); //, Framebuffer_color::white);
    display->show();
    sleep_ms(1000);
    display->vline(64, 0, 64);  //, Framebuffer_color::white);
    display->vline(127, 0, 64); //, Framebuffer_color::white);
    display->show();
    sleep_ms(1000);
}

void test_fb_rect(SSD1306 *display)
{
    display->clear_buffer_and_show_full_screen();
    display->rect(0, 0, 128, 64); //, false, Framebuffer_color::white);
    display->show();
    sleep_ms(1000);
    display->rect(10, 10, 108, 44, true); //, false, Framebuffer_color::white);
    display->show();
    sleep_ms(2000);
}
void test_fb_in_fb(SSD1306 *display)
{
    display->clear_buffer_and_show_full_screen();
    display->rect(0, 0, SSD1306_WIDTH, SSD1306_HEIGHT); //, false, Framebuffer_color::white);
    display->rect(10, 10, 108, 44, true);               //, true, Framebuffer_color::black);
    display->line(5, 60, 120, 5, Framebuffer_color::black);
    display->show();
    sleep_ms(1000);
    uint8_t small_frame_x_anchor = 20;
    uint8_t small_frame_y_anchor = 20;
    uint8_t small_frame_width = 88;
    uint8_t small_frame_height = 25;
    Framebuffer small_frame = Framebuffer(small_frame_width, small_frame_height, Framebuffer_format::MONO_VLSB);
    small_frame.fill(Framebuffer_color::black);
    small_frame.line(5, 5, 80, 20); // point coordinates are relative to the local frame
    small_frame.circle(8, 44, 12);
    display->show(&small_frame, small_frame_x_anchor, small_frame_y_anchor);
    sleep_ms(1000);
}

void test_fb_circle(SSD1306 *display)
{
    display->clear_buffer_and_show_full_screen();
    display->circle(50, 63, 31);
    display->show();
    sleep_ms(1000);
    display->circle(20, 64, 32, true);
    display->show();
    sleep_ms(2000);
}

int main()
{
    stdio_init_all();
    // create I2C bus hw peripheral and display
    hw_I2C_master master = hw_I2C_master(cfg_i2c);
    SSD1306 display = SSD1306(&master, cfg_ssd1306);

    while (true)
    {
        test_blink(&display);
        test_contrast(&display);
        test_addressing_mode(&display);
        test_scrolling(&display);

        test_fb_line(&display);
        test_outofframe_line(&display);
        test_fb_hline(&display);
        test_fb_vline(&display);
        test_fb_rect(&display);
        test_fb_circle(&display);
        test_fb_in_fb(&display);
    }

    return 0;
}
