#include "widget.h"

#define MAX_LEVEL_VALUE_SIZE 5

uint8_t Bar::convert_level_to_px(int level)
{
    uint8_t position = level * level_coef + level_offset;
    position = std::min(px_max, std::max(px_min, position));
    return position;
}

Bar::Bar(bar_widget_config_t config) : Framebuffer(config.width, config.height)
{
    this->config = config;

    if (config.draw_value)
        value_max_width = MAX_LEVEL_VALUE_SIZE * config.font[FONT_WIDTH];
    else
        value_max_width = 0;

    px_max = frame_width;
    px_min = value_max_width;
    level_coef = (float)(px_max - px_min) / (config.level_max - config.level_min);
    level_offset = px_max - level_coef * config.level_max;
    reset_px();
}

Bar::~Bar()
{
}

void Bar::reset_px()
{
    if (config.level_max * config.level_min < 0)
        level = 0;
    else if (config.level_max < 0)
        level = config.level_max;
    else
        level = config.level_min;
    px = convert_level_to_px(level);
    // printf("px: %d, level: %d\n", px, level);
}

void Bar::increment_level()
{
    level++;
    level = std::min(config.level_max, std::max(config.level_min, level));
    px = convert_level_to_px(level);
    // printf("px: %d, level: %d\n", px, level);
}

void Bar::decrement_level()
{
    level--;
    level = std::min(config.level_max, std::max(config.level_min, level));
    px = convert_level_to_px(level);
    // printf("px: %d, level: %d\n", px, level);
}

void Bar::draw()
{
    rect(0, 0, frame_width, frame_height, true, Framebuffer_color::black); // clear the full framebuffer
    if (config.draw_value)
    {
        this->clear_text_buffer();
        sprintf(this->text_buffer, "%d", level);
        print_text();
    }
    if (config.draw_border)
        rect(px_min, 0, px_max - px_min, frame_height);
        

    uint8_t bar_start, bar_end;
    if (level >= 0)
    {
        bar_start = convert_level_to_px(0);
        bar_end = px;
    }
    else
    {
        bar_start = px;
        bar_end = convert_level_to_px(0);
    }

    if (level == 0)
        rect(bar_start, 0, 1, frame_height, true);
    else
        rect(bar_start, 0, bar_end - bar_start, frame_height, true);
}
