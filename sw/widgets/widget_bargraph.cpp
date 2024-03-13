#include "widget_bargraph.h"

W_HBargraph::W_HBargraph(UI_DisplayDevice *screen, BargraphDisplayedObject *displayed_values, config_bargraph_widget_t cnf_bargraph)
    : UI_Widget(0, cnf_bargraph.bargraph_width, cnf_bargraph.bargraph_height,
                cnf_bargraph.bargraph_anchor_x, cnf_bargraph.bargraph_anchor_y,
                cnf_bargraph.format, cnf_bargraph.txt_cnf),
      UI_ControlledObject(0, 0, cnf_bargraph.bargraph_bin_number - 1)
{
    this->screen_framebuffer = screen;
    this->displayed_values = displayed_values;

    this->with_status_flag = cnf_bargraph.with_status_flag;

    this->with_border = cnf_bargraph.with_border;
    this->bargraph_bin_number = cnf_bargraph.bargraph_bin_number;
    this->bargraph_bin_spacing = cnf_bargraph.bargraph_bin_spacing;
    this->bargraph_bin_flag_width = cnf_bargraph.bargraph_bin_flag_width;

    this->border_width = (with_border) ? cnf_bargraph.border_width : 0;
    widget_start_x = border_width;
    widget_start_y = border_width;
    widget_width = frame_width - 2 * border_width;
    widget_height = frame_height - 2 * border_width;

    bar_height = std::max(5, widget_height / bargraph_bin_number);
    widget_height = bar_height * bargraph_bin_number;

    px_max = frame_width - border_width;
    px_min = border_width + bargraph_bin_flag_width + bargraph_bin_spacing;

    level_coef = (float)(px_max - px_min) / (displayed_values->max_value - displayed_values->min_value);
    level_offset = px_max - level_coef * displayed_values->max_value;
}
W_HBargraph::~W_HBargraph()
{
}

uint8_t W_HBargraph::convert_level_value_to_px(int level)
{
    uint8_t position = level * level_coef + level_offset;
    position = std::min(px_max, std::max(px_min, position));
    return position;
}

void W_HBargraph::draw()
{
    for (int i = 0; i < this->bargraph_bin_number; i++)
    {

        // draw_bar(i, bar_anchor_x,  i * bar_height, bar_width, bar_height, (i==value));// HACK draw border if bar under focus
        draw_bar(i, (i == current_active_index)); // HACK draw border if bar is selected
    }
    if (with_border)
        draw_border();
}

void W_HBargraph::draw_bar(uint8_t bin_number, bool bar_with_border)
{
    uint8_t bar_start_y = bin_number * bar_height;
    uint8_t bar_width = px_max - px_min;
    rect(widget_start_x, bar_start_y, widget_width, bar_height, true, Framebuffer_color::black); // erase the previous bar area
    if (with_status_flag)
        draw_status_flag(bin_number);

    uint8_t px = convert_level_value_to_px(this->displayed_values->values[bin_number]);

    if (bar_with_border)
        rect(widget_start_x + bargraph_bin_flag_width + bargraph_bin_spacing, bar_start_y, bar_width, bar_height, false);

    uint8_t bar_start;
    uint8_t bar_end;
    if (this->displayed_values->values[bin_number] >= 0)
    {
        bar_start = convert_level_value_to_px(0);
        bar_end = px;
    }
    else
    {
        bar_start = px;
        bar_end = convert_level_value_to_px(0);
    }

    if (this->displayed_values->values[bin_number] == 0)
        rect(bar_start, bar_start_y + bargraph_bin_spacing, 1, bar_height - bargraph_bin_spacing, true);
    else
        rect(bar_start, bar_start_y + bargraph_bin_spacing, bar_end - bar_start, bar_height - 2 * bargraph_bin_spacing, true);
}

void W_HBargraph::set_value_clipped(int new_value)
{
    this->value = std::min(max_value, std::max(min_value, new_value));
    status_has_changed = true;
}

void W_HBargraph::draw_border()
{
    rect(0, 0, frame_width, frame_height);
}

void W_HBargraph::process_control_event(ControlEvent event)
{
    switch (event)
    {
    case ControlEvent::NOOP:

        break;
    case ControlEvent::PUSH:
        // printf("event PUSH: %d\n", event);
        break;
    case ControlEvent::DOUBLE_PUSH:
        /* code */
        break;
    case ControlEvent::LONG_PUSH:
        this->set_value_clipped(0);
        // printf("event LONG_PUSH: %d\n", event);
        break;
    case ControlEvent::RELEASED_AFTER_LONG_TIME:
        // printf("event RELEASED_AFTER_LONG_TIME: %d\n", event);
        break;
    case ControlEvent::RELEASED_AFTER_SHORT_TIME:
        // printf("event RELEASED_AFTER_SHORT_TIME: %d\n", event);
        current_active_index = value;
        break;
    case ControlEvent::INCREMENT:
        increment_value();
        // printf("event INCREMENT: %d\n", value);
        break;
    case ControlEvent::DECREMENT:
        decrement_value();
        // printf("event DECREMENT: %d\n", value);
        break;

    default:
        break;
    }
}

void W_HBargraph::refresh()
{
    draw();
    this->screen_framebuffer->show(this, this->anchor_x, this->anchor_y);
}

void W_HBargraph::draw_status_flag(uint8_t bin_number)
{
    uint8_t bar_start_y = bin_number * bar_height;
    if (bin_number == current_active_index)
    {
        rect(widget_start_x, bar_start_y, bargraph_bin_flag_width, bar_height, true);
    }
    if (bin_number == value)
    {
        bool is_active = (bin_number == current_active_index) ? true : false;              // HACK draw filled square if bar is selected, otherwise draw empty square
        rect(widget_start_x, bar_start_y, bargraph_bin_flag_width, bar_height, is_active); // hack
    }
}

//---------------------------------------------------------------------

BargraphDisplayedObject::BargraphDisplayedObject(uint8_t id, int min_value, int max_value)
{
    this->min_value = min_value;
    this->max_value = max_value;
}

BargraphDisplayedObject::~BargraphDisplayedObject()
{
}

void BargraphDisplayedObject::set_value_clipped(uint8_t index, int new_value)
{
    this->values[index] = std::min(max_value, std::max(min_value, new_value));
}
