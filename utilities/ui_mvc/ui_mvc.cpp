#include "ui_mvc.h"

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

UI_ControlledObject::UI_ControlledObject(uint8_t id)
{
    this->id = id;
}

UI_ControlledObject::~UI_ControlledObject()
{
}

void UI_ControlledObject::clear_change_flag()
{
    has_changed = false;
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
