#include "ui_mvc.h"

UI_Controller::UI_Controller()
{
}

UI_Controller::~UI_Controller()
{
    delete this->controlled_object;
}

void UI_Controller::set_controlled_object(UI_ControlledObject *cntrl_obj)
{
    this->controlled_object = cntrl_obj;
}

UI_ControlledObject::UI_ControlledObject()
{
}

UI_ControlledObject::~UI_ControlledObject()
{
    this->controllers.clear();
    this->widgets.clear();
}

void UI_ControlledObject::add_widget(UI_Widget *widget)
{
    this->widgets[widget->id] = widget;
}

void UI_ControlledObject::add_controller(UI_Controller *controller)
{
    this->controllers[controller->id] = controller;
}

UI_Widget::UI_Widget()
{
}

UI_Widget::~UI_Widget()
{
    this->displayed_objects.clear();
}

void UI_Widget::add_displayed_object(UI_ControlledObject *displayed_object)
{
    this->displayed_objects[displayed_object->id] = displayed_object;
}
