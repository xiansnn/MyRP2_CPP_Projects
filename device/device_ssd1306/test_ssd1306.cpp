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
void test_line(SSD1306 *display)
{
    int y0,x1,y1;
    display->clear_buffer_and_show_GDDRAM();
    x1=64;
    y1=70;
    y0=-10;

    for (int x = -10; x < 138; x++)
    {
        Framebuffer_color c = Framebuffer_color::white;
        display->line(x, y0, x1, y1, c);
        display->show();
        c = Framebuffer_color::black;
        display->line(x, 0, SSD1306_WIDTH - 1 - x, SSD1306_HEIGHT - 1, c);
        display->show();
    }
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
    display->clear_buffer_and_show_GDDRAM();
    display->circle(50, 63, 31);
    display->show();
    sleep_ms(1000);
    display->circle(20, 64, 32, true);
    display->show();
    sleep_ms(2000);
}

void test_text_and_graph(SSD1306 *display)
{
    display->clear_buffer_and_show_GDDRAM();
    uint8_t w = font_8x8[FONT_WIDTH];
    uint8_t h = font_8x8[FONT_HEIGHT];

    // draw titles
    frame_data_t title_area{
        .anchor_x = 0,
        .anchor_y = h * 6,
        .width = w * 8,
        .height = h * 2};
    Framebuffer title = Framebuffer(title_area.width, title_area.height, Framebuffer_format::MONO_VLSB);
    title.text(font_8x8, "ROLL:", 0, 0);
    title.text(font_8x8, "PITCH:", 0, h);
    display->show(&title, title_area.anchor_x, title_area.anchor_y);

    // draw values
    frame_data_t values_area{
        .anchor_x = w * 8,
        .anchor_y = h * 6,
        .width = w * 8,
        .height = h * 2};
    Framebuffer values = Framebuffer(values_area.width, values_area.height, Framebuffer_format::MONO_VLSB);

    // draw graph
    frame_data_t graph_area{
        .anchor_x = 16,
        .anchor_y = 0,
        .width = w * 12,
        .height = h * 5};
    Framebuffer graph = Framebuffer(graph_area.width, graph_area.height, Framebuffer_format::MONO_VLSB);
    graph.fill(Framebuffer_color::black);
    display->show(&graph, graph_area.anchor_x, graph_area.anchor_y);

    int roll, pitch;

    for (int i = -90; i < 90; i++)
    {
        values.clear_buffer();
        roll = i;
        // pitch = i;
        pitch = i / 3;
        std::string roll_str = std::to_string(roll) + " " + DEGREE;
        std::string pitch_str = std::to_string(pitch) + " " + DEGREE;
        values.text(font_8x8, roll_str, 0, 0);
        values.text(font_8x8, pitch_str, 0, h);

        float xc = graph_area.width / 2;
        float yc = graph_area.height / 2;
        float yl = graph_area.height / 2 - pitch;
        float radius = yc - 2; // radius -2 to fit inside the rectangle

        // float tan_roll = tan(std::numbers::pi / 180.0 * roll);
        float sin_roll = sin(std::numbers::pi / 180.0 * roll);
        float cos_roll = cos(std::numbers::pi / 180.0 * roll);
        // float dy1 = xc * tan_roll;
        int x0 = xc - radius * cos_roll;
        int y0 = yl - radius * sin_roll;
        int x1 = xc + radius * cos_roll;
        int y1 = yl + radius * sin_roll;
        graph.rect(0, 0, graph_area.width, graph_area.height); // point coordinates are relative to the local frame
        graph.circle(radius, xc, yl);
        graph.line(x0, y0, x1, y1);
        display->show(&graph, graph_area.anchor_x, graph_area.anchor_y);
        display->show(&values, values_area.anchor_x, values_area.anchor_y);
        graph.line(x0, y0, x1, y1, Framebuffer_color::black);
        graph.circle(radius, xc, yl, false, Framebuffer_color::black);
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
            s = c;
            display->text(font_8x8, s, x, y);
            display->show();
        }
    }
    display->clear_buffer_and_show_GDDRAM();
    sleep_ms(1000);
    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 16; j++)
        {
            x = j * font_8x8[FONT_WIDTH];
            y = i * font_8x8[FONT_HEIGHT];
            c += 1;
            s = c;
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
    pr_D4.lo(); // 25 ms

    uint8_t w = font_8x8[FONT_WIDTH];
    uint8_t h = font_8x8[FONT_HEIGHT];
    uint8_t nb_text_line = SSD1306_HEIGHT / h;

    for (int i = 0; i < nb_text_line; i++)
    {
        frame_data_t data = {
            .anchor_x = 120,
            .anchor_y = i * h,
            .width = w,
            .height = h};
        Framebuffer degree_unit = Framebuffer(w, h, Framebuffer_format::MONO_VLSB);
        degree_unit.text(font_8x8, DEGREE, 0, 0);
        display->show(&degree_unit, data);
    }

    for (int i = 0; i < nb_text_line; i++)
    {
        frame_data_t title_frame = {
            .anchor_x = 0,
            .anchor_y = i * h,
            .width = w * 2,
            .height = h};
        Framebuffer title = Framebuffer(title_frame.width, title_frame.height, Framebuffer_format::MONO_VLSB);
        title.text(font_8x8, std::to_string(i) + ":", 0, 0);
        display->show(&title, title_frame);
    }

    Framebuffer *value_frames[nb_text_line];
    frame_data_t data[nb_text_line];
    for (int i = 0; i < nb_text_line; i++)
    {
        data[i] = {
            .anchor_x = w * 2,
            .anchor_y = i * h,
            .width = w * 13,
            .height = h};
        value_frames[i] = new Framebuffer(data[i].width, data[i].height, Framebuffer_format::MONO_VLSB);
    }
    std::ostringstream str_values[nb_text_line];
    for (int value = -180; value < 180; value += 1)
    {
        uint8_t value_length = std::to_string(value).length();
        for (int i = 0; i < nb_text_line; i++)
        {
            if (i == 0)
            {
                pr_D5.hi();
                value_frames[i]->text(font_8x8, std::to_string(value_length), 15, 0);
                pr_D5.lo(); // 323 us
                pr_D7.hi();
                display->show(value_frames[i], data[i]);
                pr_D7.lo(); // 3.27 ms
            }
            else
            {
                pr_D6.hi();
                str_values[i].width(13);
                // (value < 100) ? str_values[i].precision(3) : str_values[i].precision(4);
                str_values[i].precision((value < 0) ? value_length + 1 : value_length + 2);
                str_values[i].fill('_');
                str_values[i].setf(std::ios_base::right, std::ios_base::adjustfield);
                str_values[i].setf(std::ios_base::showpos);
                str_values[i] << (float)value;
                value_frames[i]->text(font_8x8, str_values[i].str(), 0, 0);
                pr_D6.lo(); // 395 us pour les chaines 13 char et 247 us pour les chaines 7 char
                pr_D7.hi();
                display->show(value_frames[i], data[i]);
                pr_D7.lo();
            }
            value_frames[i]->clear_buffer();
            str_values[i].str("");
        }
        sleep_ms(1);
    }

    for (int i = 0; i < nb_text_line; i++)
        delete value_frames[i];

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
        test_line(&display);
        // test_fb_hline(&display);
        // test_fb_vline(&display);
        // test_fb_rect(&display);
        // test_fb_circle(&display);
        // test_fb_in_fb(&display);
        // test_text_and_graph(&display);
        // test_full_screen_text(&display);
        // test_text_stream(&display);
    }

    return 0;
}
