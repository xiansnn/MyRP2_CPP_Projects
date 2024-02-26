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
    active_controlled_object->update_status(ControlledObjectStatus::HAS_FOCUS);
    this->active_controlled_object = cntrl_obj;
    active_controlled_object->update_status(ControlledObjectStatus::IS_ACTIVE);
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

bool UI_ControlledObject::has_status_changed()
{
    return status_has_changed;
}

void UI_ControlledObject::update_status(ControlledObjectStatus new_status)
{
    this->status_has_changed = (this->status != new_status) ? true : false;
    this->status = new_status;
}

ControlledObjectStatus UI_ControlledObject::get_status()
{
    return this->status;
}

void UI_ControlledObject::clear_status_change_flag()
{
    this->status_has_changed = false;
}

UI_Widget::UI_Widget(uint8_t id, size_t width, size_t height, uint8_t anchor_x, uint8_t anchor_y,
                     Framebuffer_format format, config_framebuffer_text_t txt_cnf) : Framebuffer(width, height, format, txt_cnf)
{
    this->id = id;
    this->anchor_x = anchor_x;
    this->anchor_y = anchor_y;
}

UI_Widget::~UI_Widget()
{
}

bool UI_Widget::refresh_requested()
{
    return (active_displayed_object->has_status_changed()) ? true : false;
}

void UI_Widget::refresh_done()
{
    active_displayed_object->clear_status_change_flag(); //    status_has_changed = false;
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
    status_has_changed = true;
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
