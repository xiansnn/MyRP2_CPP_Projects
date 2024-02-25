#include "ui_mvc.h"
#include <bits/stl_algobase.h>

UI_Controller::UI_Controller(uint8_t id)
{
    this->id = id;
}

UI_Controller::~UI_Controller()
{
}

void UI_Controller::set_active_controlled_object(UI_ControlledObject *cntrl_obj)
{
    this->active_controlled_object->set_active_status(false);
    this->active_controlled_object = cntrl_obj;
    this->active_controlled_object->set_active_status(true);
}

UI_ControlledObject *UI_Controller::get_active_controlled_object()
{
    return this->active_controlled_object;
}

UI_ControlledObject::UI_ControlledObject(uint8_t id, int min_value, int max_value, int increment)
{
    this->value = 0;
    this->id = id;
    this->min_value = min_value;
    this->max_value = max_value;
    this->increment = increment;
}

UI_ControlledObject::~UI_ControlledObject()
{
}

void UI_ControlledObject::set_focus_status(bool value)
{
    this->status_has_changed = (has_focus != value ) ? true : false;
    this->has_focus = value;
}

bool UI_ControlledObject::get_focus_status()
{
    clear_status_change_flag();
    return this->has_focus;
}

void UI_ControlledObject::set_active_status(bool value)
{
    this->status_has_changed = (is_active !=  value ) ? true : false;
    this->is_active = value;
}

bool UI_ControlledObject::get_active_status()
{
    return is_active;
}

void UI_ControlledObject::clear_status_change_flag()
{
    this->status_has_changed = false;
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

bool UI_Widget::refresh_requested()
{
    return (active_displayed_object->status_has_changed or active_displayed_object->value_has_changed) ? true : false;
}

void UI_Widget::refresh_done()
{
    active_displayed_object->clear_status_change_flag();//    status_has_changed = false;
    active_displayed_object->clear_value_change_flag();//    value_has_changed = false;
}

void UI_Widget::set_active_displayed_object(UI_ControlledObject *displayed_object)
{
    this->active_displayed_object = displayed_object;
}

UI_ControlledObject *UI_Widget::get_active_displayed_object()
{
    return this->active_displayed_object;
}

void UI_ControlledObject::set_value_clipped(int new_value)
{
    this->value = std::min(max_value, std::max(min_value, new_value));
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

void UI_ControlledObject::set_min_value(int value)
{
    this->min_value = value;
}

int UI_ControlledObject::get_max_value()
{
    return max_value;
}

void UI_ControlledObject::set_max_value(int value)
{
    this->max_value = value;
}
