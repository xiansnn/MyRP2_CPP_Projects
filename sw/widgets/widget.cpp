#include "widget.h"

Bar::Bar(bar_widget_config_t config) : Framebuffer(config.width, config.height)
{
    this->bar_config = config;
    this->bar_cursor_coef = this->frame_width / (bar_config.cursor_max - bar_config.cursor_min);
    this->frame_width = (bar_config.cursor_max - bar_config.cursor_min) * this->bar_cursor_coef;
    this->bar_zero = std::abs(bar_config.cursor_min * this->bar_cursor_coef);
    this->cursor = 0;
    this->bar_size = 0;
}

Bar::~Bar()
{
}

void Bar::reset_cursor()
{
    this->cursor = 0;
}

void Bar::increment_cursor()
{
    cursor++;
    cursor = std::min(bar_config.cursor_max, std::max(bar_config.cursor_min, cursor));
}

void Bar::decrement_cursor()
{
    cursor--;
    cursor = std::min(bar_config.cursor_max, std::max(bar_config.cursor_min, cursor));
}

void Bar::draw()
{
    bar_size = std::abs(cursor * bar_cursor_coef);
    rect(0, 0, frame_width, frame_height, true, Framebuffer_color::black);
    if (this->bar_config.border)
        rect(0, 0, frame_width, frame_height);

    if (cursor == 0)
        rect(bar_zero, 0, 1, frame_height, true);

    else if (cursor > 0)
        rect(bar_zero, 0, bar_size, frame_height, true);

    else
        rect(bar_zero - bar_size, 0, bar_size, frame_height, true);
}
