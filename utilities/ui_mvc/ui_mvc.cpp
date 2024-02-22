#include "ui_mvc.h"
#include <bits/stl_algobase.h>

UI_Controller::UI_Controller(uint8_t id)
{
    this->id = id;
}

UI_Controller::~UI_Controller()
{
}

UI_ControlledObject *UI_Controller::set_active_controlled_object(UI_ControlledObject *cntrl_obj)
{
    this->active_controlled_object = cntrl_obj;
    return this->active_controlled_object;
}

UI_ControlledObject::UI_ControlledObject(uint8_t id, int min_value, int max_value, int increment)
{
    this->value = 0;
    this->id = id;
    this->min_value = min_value;
    this->max_value = max_value;
    this->increment_val = increment;
}

UI_ControlledObject::UI_ControlledObject(uint8_t id)
{
    this->id = id;
}

UI_ControlledObject::~UI_ControlledObject()
{
}

void UI_ControlledObject::clear_value_change_flag()
{
    value_has_changed = false;
}

UI_Widget::UI_Widget(uint8_t id)
{
    this->id = id;
}

UI_Widget::~UI_Widget()
{
}

UI_ControlledObject *UI_Widget::set_active_displayed_object(UI_ControlledObject *displayed_object)
{
    this->active_displayed_object = displayed_object;
    return this->active_displayed_object;
}

void UI_ControlledObject::set_value(int new_value)
{
    this->value = std::min(max_value, std::max(min_value, new_value));
    value_has_changed = true;
}

void UI_ControlledObject::reset()
{
    this->value = std::min(max_value, std::max(min_value, 0));
    value_has_changed = true;
}

int UI_ControlledObject::get_value()
{
    return value;
}

int UI_ControlledObject::get_min_value()
{
    return min_value;
}

int UI_ControlledObject::get_max_value()
{
    return max_value;
}
