#include "widget_bargraph.h"

W_HBargraph::W_HBargraph(AbstractDisplayDevice *display_screen, Bargraph *displayed_values, config_bargraph_widget_t cnf_bargraph)
    : AbstractWidget(display_screen, cnf_bargraph.bargraph_width, cnf_bargraph.bargraph_height,
                     cnf_bargraph.bargraph_anchor_x, cnf_bargraph.bargraph_anchor_y, cnf_bargraph.with_border, cnf_bargraph.border_width,
                     cnf_bargraph.format, cnf_bargraph.txt_cnf),
      UI_ControlledObject(0, 0, cnf_bargraph.bargraph_bin_number - 1)
{
    this->displayed_bargraph = displayed_values;

    this->with_threshold = cnf_bargraph.with_threshold;
    this->threshold = this->displayed_bargraph->max_value * 0.9;
    this->threshold_increment = this->displayed_bargraph->max_value * 0.01;

    this->bargraph_bin_number = cnf_bargraph.bargraph_bin_number;
    this->bargraph_bin_spacing = cnf_bargraph.bargraph_bin_spacing;

    this->with_status_flag = cnf_bargraph.with_status_flag;
    this->bargraph_bin_flag_width = (with_status_flag) ? cnf_bargraph.bargraph_bin_flag_width + bargraph_bin_spacing : 0;
    this->status_flag_mode = cnf_bargraph.status_flag_mode;

    bar_height = std::max(5, widget_height / bargraph_bin_number); // round sup
    widget_height = bar_height * bargraph_bin_number;

    px_max = frame_width - widget_border_width;
    px_min = widget_border_width + bargraph_bin_flag_width;
    bar_width = px_max - px_min;

    level_coef = (float)(px_max - px_min) / (displayed_values->max_value - displayed_values->min_value);
    level_offset = px_max - level_coef * displayed_values->max_value;
}
W_HBargraph::~W_HBargraph()
{
}

void W_HBargraph::draw_threshold()
{
    uint8_t threshold_position = convert_level_value_to_px(this->threshold);
    if (control_mode == ControlMode::BAND_CONTROL)
    {
        vline(threshold_position, 0, frame_height);
    }
    else if (control_mode == ControlMode::THRESHOLD_CONTROL)
    {
        vline(threshold_position, 0, frame_height, AbstractWidget::blinking_us(100000));
    }
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
        draw_bar(i);
        if (with_status_flag)
            draw_status_flag(i);
    }
    if (with_threshold)
        draw_threshold();
}
void W_HBargraph::increment_threshold()
{
    threshold += threshold_increment;
    threshold = std::min(displayed_bargraph->max_value, std::max(displayed_bargraph->min_value, threshold));
}
void W_HBargraph::decrement_threshold()
{
    threshold -= threshold_increment;
    threshold = std::min(displayed_bargraph->max_value, std::max(displayed_bargraph->min_value, threshold));
}
void W_HBargraph::draw_status_flag(uint8_t bin_number)
{
    bool is_active = (bin_number == current_active_index) ? true : false; // cannot use UI_ControlledObjectstatus because we need a status for each bar
    bool is_under_focus = (bin_number == value) ? true : false;
    uint8_t bar_start_y = bin_number * bar_height;
    switch (this->status_flag_mode)
    {
    case StatusFlagMode::BORDER_FLAG:
        if (is_active)
            rect(widget_start_x + bargraph_bin_flag_width, bar_start_y, bar_width - 1, bar_height, false); // HACK
        if (is_under_focus)
            rect(widget_start_x, bar_start_y, bargraph_bin_flag_width - bargraph_bin_spacing, bar_height, true);
        break;
    case StatusFlagMode::SQUARE_FLAG:
        if (is_active)
            rect(widget_start_x, bar_start_y, bargraph_bin_flag_width - bargraph_bin_spacing, bar_height, true);
        if (is_under_focus)
            rect(widget_start_x, bar_start_y, bargraph_bin_flag_width - bargraph_bin_spacing, bar_height, false);
        break;
    default:
        break;
    }
}
void W_HBargraph::draw_bar(uint8_t bin_number)
{
    uint8_t bar_start_y = bin_number * bar_height;
    rect(0, bar_start_y, frame_width, bar_height, true, Framebuffer_color::black); // erase the bar area

    uint8_t px = convert_level_value_to_px(this->displayed_bargraph->values[bin_number]);
    uint16_t p0 = convert_level_value_to_px(0);

    uint8_t bar_start;
    uint8_t bar_end;
    if (this->displayed_bargraph->values[bin_number] >= 0)
    {
        bar_start = p0;
        bar_end = px;
    }
    else
    {
        bar_start = px;
        bar_end = p0;
    }

    if (this->displayed_bargraph->values[bin_number] == 0)
        rect(bar_start, bar_start_y + bargraph_bin_spacing, 1, bar_height - bargraph_bin_spacing, true);
    else
        rect(bar_start, bar_start_y + bargraph_bin_spacing, bar_end - bar_start, bar_height - 2 * bargraph_bin_spacing, true);
}

void W_HBargraph::set_value_clipped(int new_value)
{
    this->value = std::min(max_value, std::max(min_value, new_value));
    this->displayed_object->set_change_flag();
}

void W_HBargraph::process_control_event(ControlEvent event)
{
    switch (event)
    {
    case ControlEvent::NOOP:
        break;
    case ControlEvent::PUSH:
        break;
    case ControlEvent::DOUBLE_PUSH:
        break;
    case ControlEvent::LONG_PUSH:
        if (with_threshold)
        {
            if (control_mode == ControlMode::BAND_CONTROL)
                control_mode = ControlMode::THRESHOLD_CONTROL;
            else if (control_mode == ControlMode::THRESHOLD_CONTROL)
                control_mode = ControlMode::BAND_CONTROL;
        }
        else
            control_mode = ControlMode::BAND_CONTROL;

        break;
    case ControlEvent::RELEASED_AFTER_LONG_TIME:
        break;
    case ControlEvent::RELEASED_AFTER_SHORT_TIME:
        current_active_index = value;
        break;
    case ControlEvent::INCREMENT:
        switch (control_mode)
        {
        case ControlMode::BAND_CONTROL:
            increment_value();
            break;
        case ControlMode::THRESHOLD_CONTROL:
            increment_threshold();
            break;
        default:
            break;
        }
        break;
    case ControlEvent::DECREMENT:
        switch (control_mode)
        {
        case ControlMode::BAND_CONTROL:
            decrement_value();
            break;
        case ControlMode::THRESHOLD_CONTROL:
            decrement_threshold();
            break;
        default:
            break;
        }
        break;
    case ControlEvent::TIME_OUT:
        break;
    }
}
