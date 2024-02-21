#include "print_console_line.h"
#include <string>

DisplayEncoderOnTerminal::DisplayEncoderOnTerminal(uint8_t id) : UI_Widget(id)
{
}

DisplayEncoderOnTerminal::~DisplayEncoderOnTerminal()
{
}

void DisplayEncoderOnTerminal::draw()
{
    int display_value = slope * active_displayed_object->get_value() + offset;
    printf("focus[%d]: %2d %*c\n", active_displayed_object->id, active_displayed_object->get_value(), display_value, '|');
}

UI_ControlledObject* DisplayEncoderOnTerminal::set_active_displayed_object(UI_ControlledObject *val)
{
    this->active_displayed_object = val;
    slope = (max_line_width - 1.) / (active_displayed_object->get_max_value() - active_displayed_object->get_min_value());
    offset = 1 - slope * active_displayed_object->get_min_value();
    return this->active_displayed_object;
}