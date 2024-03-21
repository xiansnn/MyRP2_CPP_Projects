#include "widget_simple_bargraph.h"

uint8_t W_SimpleHBargraph::convert_level_value_to_px(int level)
{
    uint8_t position = level * level_coef + level_offset;
    position = std::min(px_max, std::max(px_min, position));
    return position;
}

void W_SimpleHBargraph::draw()
{
    for (int i = 0; i < this->bargraph_bin_number; i++)
    {
        draw_bar(i);
    }
}

void W_SimpleHBargraph::draw_bar(uint8_t bin_number)
{
    uint8_t bar_start_y = bin_number * bar_height;
    rect(0, bar_start_y, frame_width, bar_height, true, Framebuffer_color::black); // erase the bar area

    uint8_t px = convert_level_value_to_px(this->displayed_values->values[bin_number]);
    uint16_t p0 = convert_level_value_to_px(0);

    uint8_t bar_start;
    uint8_t bar_end;
    if (this->displayed_values->values[bin_number] >= 0)
    {
        bar_start = p0;
        bar_end = px;
    }
    else
    {
        bar_start = px;
        bar_end = p0;
    }

    if (this->displayed_values->values[bin_number] == 0)
        rect(bar_start, bar_start_y + bargraph_bin_spacing, 1, bar_height - bargraph_bin_spacing, true);
    else
        rect(bar_start, bar_start_y + bargraph_bin_spacing, bar_end - bar_start, bar_height - 2 * bargraph_bin_spacing, true);
}

W_SimpleHBargraph::W_SimpleHBargraph(UI_DisplayDevice *display_screen, BargraphDisplayedObject *displayed_values, config_simple_bargraph_widget_t cnf_bargraph)
    : AbstractWidget(display_screen, cnf_bargraph.bargraph_width, cnf_bargraph.bargraph_height,
                     cnf_bargraph.bargraph_anchor_x, cnf_bargraph.bargraph_anchor_y, cnf_bargraph.with_border, cnf_bargraph.border_width,
                     cnf_bargraph.format, cnf_bargraph.txt_cnf)
{
    this->displayed_values = displayed_values;

    this->bargraph_bin_number = cnf_bargraph.bargraph_bin_number;
    this->bargraph_bin_spacing = cnf_bargraph.bargraph_bin_spacing;

    bar_height = std::max(5, widget_height / bargraph_bin_number); // round sup
    widget_height = bar_height * bargraph_bin_number;

    px_max = frame_width - border_width;
    px_min = border_width + bargraph_bin_flag_width;
    bar_width = px_max - px_min;

    level_coef = (float)(px_max - px_min) / (displayed_values->max_value - displayed_values->min_value);
    level_offset = px_max - level_coef * displayed_values->max_value;
}

W_SimpleHBargraph::~W_SimpleHBargraph()
{
}

//---------------------------------------------------------------------
BargraphDisplayedObject::BargraphDisplayedObject(int min_value, int max_value)
{
    this->min_value = min_value;
    this->max_value = max_value;
}

BargraphDisplayedObject::~BargraphDisplayedObject()
{
}
