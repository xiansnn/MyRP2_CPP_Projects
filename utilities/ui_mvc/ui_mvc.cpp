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

UI_ControlledObject::UI_ControlledObject(uint8_t id, int min_value, int max_value, bool wrap, int increment)
{
    this->value = 0;
    this->id = id;
    this->min_value = min_value;
    this->max_value = max_value;
    this->increment = increment;
    this->wrap = wrap;
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
    active_displayed_object->clear_status_change_flag();
}

void UI_Widget::set_active_displayed_object(UI_ControlledObject *displayed_object)
{
    this->active_displayed_object = displayed_object;
}

UI_ControlledObject *UI_Widget::get_active_displayed_object()
{
    return this->active_displayed_object;
}

void UI_ControlledObject::increment_value()
{
    value += increment;
    if ((wrap) and (value > max_value))
        value = min_value;
    this->value = std::min(max_value, std::max(min_value, value));
    status_has_changed = true;
}

void UI_ControlledObject::decrement_value()
{
    value -= increment;
    if ((wrap) and (value < min_value))
        value = max_value;
    this->value = std::min(max_value, std::max(min_value, value));
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

UI_DisplayDevice::UI_DisplayDevice(size_t width, size_t height,
                                   Framebuffer_format format, config_framebuffer_text_t txt_cnf) : Framebuffer(width, height, format, txt_cnf)
{
}

UI_DisplayDevice::~UI_DisplayDevice()
{
}
UI_WidgetManager::UI_WidgetManager(UI_DisplayDevice *screen) : UI_ControlledObject(FOCUS_MANAGER_ID), UI_Controller(FOCUS_MANAGER_ID)
{
    this->screen_framebuffer = screen;
    min_value = 0;
    value = 0;
    active_controlled_object = this;
}

UI_WidgetManager::~UI_WidgetManager()
{
}

void UI_WidgetManager::refresh()
{
    for (UI_Widget *w : widgets)
    {
        if (w->refresh_requested())
        {
            w->draw();
            if (this->screen_framebuffer != nullptr)
            {
                this->screen_framebuffer->show(w, w->anchor_x, w->anchor_y);
            }

            w->refresh_done();
        }
    }
};

void UI_WidgetManager::add_controlled_object(UI_ControlledObject *cntrl_obj) //
{
    this->controlled_objects.push_back(cntrl_obj);
    this->max_value = controlled_objects.size() - 1;
    this->controlled_object_under_focus = cntrl_obj;
}

void UI_WidgetManager::add_widget(UI_Widget *widget)
{
    this->widgets.push_back(widget);
}

void UI_WidgetManager::clear_active_controlled_object_change_flag()
{
    active_controlled_object_has_changed = false;
}

AbstractWidget::AbstractWidget(UI_DisplayDevice *display_screen, size_t width, size_t height, uint8_t anchor_x, uint8_t anchor_y, bool with_border, uint8_t border_width,
                               Framebuffer_format format, config_framebuffer_text_t txt_cnf) : Framebuffer(width, height, format, txt_cnf)
{
    this->display_screen = display_screen;
    this->anchor_x = anchor_x;
    this->anchor_y = anchor_y;
    this->with_border = with_border;
    this->border_width = (with_border) ? border_width : 0;

    widget_start_x = border_width;
    widget_start_y = border_width;
    widget_width = frame_width - 2 * border_width;
    widget_height = frame_height - 2 * border_width + 1;
}

AbstractWidget::~AbstractWidget()
{
}

void AbstractWidget::refresh()
{
    draw();
    if (with_border)
        draw_border();
    this->display_screen->show(this, this->anchor_x, this->anchor_y);
}

void AbstractWidget::draw_border()
{
    rect(0, 0, frame_width, frame_height);
}

AbstractControlledValue::AbstractControlledValue(/* args */)
{
}

AbstractControlledValue::~AbstractControlledValue()
{
}