#include "print_console_line.h"
#include <string>

DisplayControlledValueOnTerminal::DisplayControlledValueOnTerminal(uint8_t id) : UI_Widget(id)
{
}

DisplayControlledValueOnTerminal::~DisplayControlledValueOnTerminal()
{
}

void DisplayControlledValueOnTerminal::draw()
{
    int display_value = slope * active_displayed_object->get_value() + offset;
    printf("active_displayed_object[%d]: %2d %*c\n", active_displayed_object->id, active_displayed_object->get_value(), display_value, '|');
}

UI_ControlledObject* DisplayControlledValueOnTerminal::set_active_displayed_object(UI_ControlledObject *val)
{
    this->active_displayed_object = val;
    slope = (max_line_width - 1.) / (active_displayed_object->get_max_value() - active_displayed_object->get_min_value());
    offset = 1 - slope * active_displayed_object->get_min_value();
    return this->active_displayed_object;
}