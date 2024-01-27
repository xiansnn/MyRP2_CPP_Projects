#include "widget.h"

Bar::Bar(bar_widget_config_t config) : Framebuffer(config.width, config.height)
{
    this->bar_config = config;
    this->level_scale = this->frame_width / (bar_config.level_max - bar_config.level_min);
    this->frame_width = (bar_config.level_max - bar_config.level_min) * this->level_scale;
    this->bar_zero = std::abs(bar_config.level_min * this->level_scale);
    this->level = 0;
    this->bar_size = 0;
}

Bar::~Bar()
{
}

void Bar::reset_level()
{
    this->level = 0;
}

void Bar::increment_level()
{
    level++;
    level = std::min(bar_config.level_max, std::max(bar_config.level_min, level));
}

void Bar::decrement_level()
{
    level--;
    level = std::min(bar_config.level_max, std::max(bar_config.level_min, level));
}

void Bar::draw()
{
    bar_size = std::abs(level * level_scale);
    rect(0, 0, frame_width, frame_height, true, Framebuffer_color::black);
    if (this->bar_config.border)
        rect(0, 0, frame_width, frame_height);

    if (level == 0)
        rect(bar_zero, 0, 1, frame_height, true);

    else if (level > 0)
        rect(bar_zero, 0, bar_size, frame_height, true);

    else
        rect(bar_zero - bar_size, 0, bar_size, frame_height, true);
}
