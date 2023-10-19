#include "ssd1306.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "raspberry26x32.h"
#include "framebuffer.h"
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
    display->clear_buffer_and_show_GDDRAM();
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
    display->clear_buffer_and_show_GDDRAM();
    render_area_t area;
    // HORIZONTAL_ADDRESSING_MODE
    for (size_t i = 0; i < 4; i++)
    {
        memset(image, 0xAA, sizeof(image));
        area = SSD1306::compute_render_area(10 * i, 90 + 10 * i, 8 * i, 2 + 8 * i);
        display->show_render_area(image, area, HORIZONTAL_ADDRESSING_MODE);
        sleep_ms(1000);
        display->clear_buffer_and_show_GDDRAM();
    }
    // VERTICAL_ADDRESSING_MODE
    for (size_t i = 0; i < 4; i++)
    {
        memset(image, 0xAA, sizeof(image));
        area = SSD1306::compute_render_area(40 + 10 * i, 50 + 10 * i, 8 * i, 30 + 8 * i);
        display->show_render_area(image, area, VERTICAL_ADDRESSING_MODE);
        sleep_ms(1000);
        display->clear_buffer_and_show_GDDRAM();
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
    display->clear_buffer_and_show_GDDRAM();
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
    display->clear_buffer_and_show_GDDRAM();
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
void test_fb_line(SSD1306 *display)
{
    display->clear_buffer_and_show_GDDRAM();
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
            // display->show_render_area(display->buffer, full_screen_area);
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
    display->clear_buffer_and_show_GDDRAM();
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
    display->clear_buffer_and_show_GDDRAM();
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
    display->clear_buffer_and_show_GDDRAM();
    display->rect(0, 0, 128, 64); //, false, Framebuffer_color::white);
    display->show();
    sleep_ms(1000);
    display->rect(10, 10, 108, 44, true); //, false, Framebuffer_color::white);
    display->show();
    sleep_ms(2000);
}
void test_fb_in_fb(SSD1306 *display)
{
    display->clear_buffer_and_show_GDDRAM();
    display->rect(0, 0, 128, 64);         //, false, Framebuffer_color::white);
    display->rect(10, 10, 108, 44, true); //, true, Framebuffer_color::black);
    display->line(5, 60, 120, 5, Framebuffer_color::black);
    display->show();
    sleep_ms(1000);
    render_area_t small_frame_area = SSD1306::compute_render_area(20, 107, 20, 44);
    Framebuffer small_frame = Framebuffer(small_frame_area.width, small_frame_area.height, Framebuffer_format::MONO_VLSB);
    small_frame.fill(Framebuffer_color::black);
    small_frame.line(5, 5, 80, 20); // point coordinates are relative to the local frame
    small_frame.circle(8, 44, 12);
    display->show_render_area(small_frame.buffer, small_frame_area);
    sleep_ms(1000);
}

void test_fb_circle(SSD1306 *display)
{
    display->clear_buffer_and_show_GDDRAM();
    display->circle(50, 63, 31);
    display->show();
    sleep_ms(1000);
    display->circle(20, 64, 32, true);
    display->show();
    sleep_ms(2000);
}
void test_text(SSD1306 *display)
{
    display->clear_buffer_and_show_GDDRAM();
    render_area_t title_area = SSD1306::compute_render_area(0, 63, 40, 63);
    Framebuffer title = Framebuffer(title_area.width, title_area.height, Framebuffer_format::MONO_VLSB);
    title.text(font_8x8, "ROLL:", 0, 0);
    title.text(font_8x8, "PITCH:", 0, 16);
    display->show_render_area(title.buffer, title_area);
    sleep_ms(500);
    // draw graph
    render_area_t graph_area = SSD1306::compute_render_area(20, 107, 0, 30);
    Framebuffer graph = Framebuffer(graph_area.width, graph_area.height, Framebuffer_format::MONO_VLSB);
    graph.fill(Framebuffer_color::black);
    display->show_render_area(graph.buffer, graph_area);
    // draw values
    render_area_t values_area = SSD1306::compute_render_area(64, 127, 40, 63);
    Framebuffer values = Framebuffer(values_area.width, values_area.height, Framebuffer_format::MONO_VLSB);
    int roll, pitch;

    for (int i = -45; i < 45; i++)
    {
        values.clear_buffer();
        roll = i;
        pitch = i;
        std::string roll_str = std::to_string(roll) + " " + DEGREE;
        std::string pitch_str = std::to_string(pitch) + " " + DEGREE;
        values.text(font_8x8, roll_str, 0, 0);
        values.text(font_8x8, pitch_str, 0, 16);

        float xc = graph_area.width / 2;
        float yc = graph_area.height / 2;
        float yl = graph_area.height / 2 - pitch / 2;
        float t = tan(std::numbers::pi / 180.0 * roll);
        float dy1 = xc * t;
        int x0 = 0;
        int y0 = yl + dy1;
        int x1 = graph_area.width - 1;
        int y1 = yl - dy1;
        graph.rect(0, 0, graph_area.width, graph_area.height); // point coordinates are relative to the local frame
        graph.circle(yc, xc, yc);
        graph.line(x0, y0, x1, y1);
        display->show_render_area(graph.buffer, graph_area);
        display->show_render_area(values.buffer, values_area);
        graph.line(x0, y0, x1, y1, Framebuffer_color::black);
        sleep_ms(50);
    }
    sleep_ms(1000);
}
void test_full_screen_text(SSD1306 *display)
{
    display->clear_buffer_and_show_GDDRAM();
    uint8_t c = 31;
    std::string s{""};
    uint8_t x{0}, y{0};
    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 16; j++)
        {
            x = j * font_8x8[FONT_WIDTH];
            y = i * font_8x8[FONT_HEIGHT];
            c += 1;
            s=c;
            display->text(font_8x8, s, x, y);
            display->show();
        }
    }
    display->clear_buffer_and_show_GDDRAM();
    sleep_ms(500);
    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 16; j++)
        {
            x = j * font_8x8[FONT_WIDTH];
            y = i * font_8x8[FONT_HEIGHT];
            c += 1;
            s=c;
            display->text(font_8x8, s, x, y);
            display->show();
        }
    }
    sleep_ms(1000);
}
void test_text_stream(SSD1306 *display)
{
    pr_D4.hi();
    display->clear_buffer_and_show_GDDRAM();
    pr_D4.lo();
    pr_D4.hi();

    Framebuffer degree_unit = Framebuffer(font_8x8[FONT_WIDTH], font_8x8[FONT_HEIGHT], Framebuffer_format::MONO_VLSB);
    degree_unit.text(font_8x8, DEGREE, 0, 0);
    render_area_t unit1_area = SSD1306::compute_render_area(120, 127, 0, 7);
    display->show_render_area(degree_unit.buffer, unit1_area);

    render_area_t unit2_area = SSD1306::compute_render_area(120, 127, 8, 8 + 7);
    display->show_render_area(degree_unit.buffer, unit2_area);

    render_area_t unit3_area = SSD1306::compute_render_area(120, 127, 32, 32 + 7);
    display->show_render_area(degree_unit.buffer, unit3_area);

    render_area_t unit4_raea = SSD1306::compute_render_area(120, 127, 48, 48 + 7);
    display->show_render_area(degree_unit.buffer, unit4_raea);

    std::string roll_title = " ROLL:";
    std::string pitch_title = "PITCH:";

    Framebuffer roll_1 = Framebuffer(font_8x8[FONT_WIDTH] * 6, font_8x8[FONT_HEIGHT], Framebuffer_format::MONO_VLSB);
    render_area_t roll_1_area = SSD1306::compute_render_area(0, font_8x8[FONT_WIDTH] * 6 - 1, 0, 7);
    roll_1.text(font_8x8, roll_title, 0, 0);
    display->show_render_area(roll_1.buffer, roll_1_area);

    Framebuffer pitch_1 = Framebuffer(font_8x8[FONT_WIDTH] * 6, font_8x8[FONT_HEIGHT], Framebuffer_format::MONO_VLSB);
    render_area_t pitch_1_area = SSD1306::compute_render_area(0, font_8x8[FONT_WIDTH] * 6 - 1, 8, 8 + 7);
    pitch_1.text(font_8x8, pitch_title, 0, 0);
    display->show_render_area(pitch_1.buffer, pitch_1_area);

    Framebuffer roll_2 = Framebuffer(font_8x8[FONT_WIDTH] * 6, font_8x8[FONT_HEIGHT], Framebuffer_format::MONO_VLSB);
    render_area_t roll_2_area = SSD1306::compute_render_area(0, font_8x8[FONT_WIDTH] * 6 - 1, 32, 32 + 7);
    roll_2.text(font_8x8, roll_title, 0, 0);
    display->show_render_area(roll_2.buffer, roll_2_area);

    Framebuffer pitch_2 = Framebuffer(font_8x8[FONT_WIDTH] * 6, font_8x8[FONT_HEIGHT], Framebuffer_format::MONO_VLSB);
    render_area_t pitch_2_area = SSD1306::compute_render_area(0, font_8x8[FONT_WIDTH] * 6 - 1, 48, 48 + 7);
    pitch_2.text(font_8x8, pitch_title, 0, 0);
    display->show_render_area(pitch_2.buffer, pitch_2_area);

    Framebuffer roll_1_value = Framebuffer(font_8x8[FONT_WIDTH] * 8, font_8x8[FONT_HEIGHT], Framebuffer_format::MONO_VLSB);
    render_area_t roll_1_value_area = SSD1306::compute_render_area(font_8x8[FONT_WIDTH] * 6, font_8x8[FONT_WIDTH] * 15 - 1, 0, 7);
    Framebuffer pitch_1_value = Framebuffer(font_8x8[FONT_WIDTH] * 8, font_8x8[FONT_HEIGHT], Framebuffer_format::MONO_VLSB);
    render_area_t pitch_1_value_area = SSD1306::compute_render_area(font_8x8[FONT_WIDTH] * 6, font_8x8[FONT_WIDTH] * 15 - 1, 8, 8 + 7);
    Framebuffer roll_2_value = Framebuffer(font_8x8[FONT_WIDTH] * 8, font_8x8[FONT_HEIGHT], Framebuffer_format::MONO_VLSB);
    render_area_t roll_2_value_area = SSD1306::compute_render_area(font_8x8[FONT_WIDTH] * 6, font_8x8[FONT_WIDTH] * 15 - 1, 32, 32 + 7);
    Framebuffer pitch_2_value = Framebuffer(font_8x8[FONT_WIDTH] * 8, font_8x8[FONT_HEIGHT], Framebuffer_format::MONO_VLSB);
    render_area_t pitch_2_value_area = SSD1306::compute_render_area(font_8x8[FONT_WIDTH] * 6, font_8x8[FONT_WIDTH] * 15 - 1, 48, 48 + 7);

    std::ostringstream roll_1_str_value = std::ostringstream();
    std::ostringstream pitch_1_str_value = std::ostringstream();
    std::ostringstream roll_2_str_value = std::ostringstream();
    roll_2_str_value.setf(std::ios_base::right, std::ios_base::adjustfield);
    roll_2_str_value.setf(std::ios_base::showpos);
    roll_2_str_value.width(8);
    roll_2_str_value.precision(3);
    std::ostringstream pitch_2_str_value = std::ostringstream();
    pitch_2_str_value.copyfmt(roll_2_str_value);

    float roll, pitch;
    roll = 100;
    pitch = -123;

    roll_1_str_value << std::showpoint << std::showpos << std::setprecision(3) << std::setw(5) << std::right << roll;
    pitch_1_str_value << std::setprecision(4) << pitch;
    roll_2_str_value << roll;
    pitch_2_str_value << pitch;

    roll_1_value.text(font_8x8, roll_1_str_value.str(), 0, 0);
    display->show_render_area(roll_1_value.buffer, roll_1_value_area);

    pitch_1_value.text(font_8x8, pitch_1_str_value.str(), 0, 0);
    display->show_render_area(pitch_1_value.buffer, pitch_1_value_area);

    roll_2_value.text(font_8x8, roll_2_str_value.str(), 0, 0);
    display->show_render_area(roll_2_value.buffer, roll_2_value_area);

    pitch_2_value.text(font_8x8, pitch_2_str_value.str(), 0, 0);
    display->show_render_area(pitch_2_value.buffer, pitch_2_value_area);

    pr_D4.lo();

    // for (int i = -180; i < 180; i = i + 5)
    // {
    //     roll = i;
    //     pitch = i;

    //     pr_D6.hi();
    //     value1.clear_buffer();
    //     pr_D6.lo();
    //     sleep_us(1);
    //     pr_D6.hi();
    //     std::string roll_str = std::to_string(roll) + " " + DEGREE;
    //     std::string pitch_str = std::to_string(pitch) + " " + DEGREE;
    //     value1.text(font_8x8, roll_str, 0, 0);
    //     value1.text(font_8x8, pitch_str, 0, 9);
    //     display->show_render_area(value1.buffer, value1_area);
    //     pr_D6.lo();

    //     pr_D7.hi();
    //     value2.clear_buffer();
    //     roll2_str_value.str("");
    //     pitch2_str_value.str("");
    //     roll2_str_value << std::setprecision(3) << std::setw(5) << std::right << roll;
    //     // pitch2_str_value << pitch << std::iostream::showpos << std::iostream::right << pitch2_str_value.width(8);
    //     pitch2_str_value<<std::showpos << std::setprecision(3) << std::setw(5) << std::left << pitch; //<< std::iostream::showpos << std::iostream::right << pitch2_str_value.width(8);

    //     value2.text(font_8x8, roll2_str_value.str(), 0, 0);
    //     value2.text(font_8x8, pitch2_str_value.str(), 0, 16);
    //     display->show_render_area(value2.buffer, value2_area);
    //     pr_D7.lo();
    //     sleep_ms(100);
    // }
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
        // test_blink(&display);
        // test_contrast(&display);
        // test_addressing_mode(&display);
        // test_scrolling(&display);

        // test_fb_line(&display);
        // test_fb_hline(&display);
        // test_fb_vline(&display);
        // test_fb_rect(&display);
        // test_fb_circle(&display);
        // test_fb_in_fb(&display);
        // test_text(&display);
        test_full_screen_text(&display);
        // test_text_stream(&display);
    }

    return 0;
}
