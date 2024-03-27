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

bool UI_ControlledObject::is_refresh_requested()
{
    return refresh_requested;
}

void UI_ControlledObject::update_status(ControlledObjectStatus new_status)
{
    this->refresh_requested = (this->status != new_status) ? true : false;
    this->status = new_status;
}

ControlledObjectStatus UI_ControlledObject::get_status()
{
    return this->status;
}

void UI_ControlledObject::clear_refresh_requested_flag()
{
    this->refresh_requested = false;
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
    return (active_displayed_object->is_refresh_requested()) ? true : false;
}

void UI_Widget::refresh_done()
{
    active_displayed_object->clear_refresh_requested_flag();
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
    refresh_requested = true;
}

void UI_ControlledObject::decrement_value()
{
    value -= increment;
    if ((wrap) and (value < min_value))
        value = max_value;
    this->value = std::min(max_value, std::max(min_value, value));
    refresh_requested = true;
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

AbstractDisplayDevice::AbstractDisplayDevice(size_t width, size_t height,
                                             Framebuffer_format format, config_framebuffer_text_t txt_cnf) : Framebuffer(width, height, format, txt_cnf)
{
}

AbstractDisplayDevice::~AbstractDisplayDevice()
{
}
UI_WidgetManager::UI_WidgetManager(AbstractDisplayDevice *screen) : UI_ControlledObject(FOCUS_MANAGER_ID), UI_Controller(FOCUS_MANAGER_ID)
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
                this->screen_framebuffer->show(w, w->anchor_x, w->anchor_y);
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

AbstractWidget::AbstractWidget(AbstractDisplayDevice *_display_screen, size_t _frame_width, size_t _frame_height, uint8_t _widget_anchor_x, uint8_t _widget_anchor_y,
                               bool _widget_with_border, uint8_t _widget_border_width,
                               Framebuffer_format _framebuffer_format, config_framebuffer_text_t _framebuffer_txt_cnf) : Framebuffer(_frame_width, _frame_height, _framebuffer_format, _framebuffer_txt_cnf)
{
    this->display_screen = _display_screen;
    this->widget_anchor_x = _widget_anchor_x;
    this->widget_anchor_y = _widget_anchor_y;
    this->widget_with_border = _widget_with_border;
    this->widget_border_width = (widget_with_border) ? _widget_border_width : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = frame_width - 2 * widget_border_width;
    widget_height = frame_height - 2 * widget_border_width;
}

AbstractWidget::~AbstractWidget()
{
}

void AbstractWidget::set_displayed_object(AbstractModelObject *_displayed_object)
{
    this->displayed_object = _displayed_object;
}

void AbstractWidget::refresh()
{

    if (this->displayed_object->has_changed())
    {
        draw();
        if (widget_with_border)
            draw_border();
        this->display_screen->show(this, this->widget_anchor_x, this->widget_anchor_y);
        this->displayed_object->clear_change_flag();
    }
}

Framebuffer_color AbstractWidget::blinking_us(uint32_t blink_time)
{
    return ((time_us_32() / blink_time) % 2) ? Framebuffer_color::white : Framebuffer_color::black;
}

void AbstractWidget::draw_border()
{
    rect(0, 0, frame_width, frame_height);
}

AbstractModelObject::AbstractModelObject()
{
}

AbstractModelObject::~AbstractModelObject()
{
}

void AbstractModelObject::set_current_widget(AbstractWidget *_displaying_widget)
{
    this->displaying_widget = _displaying_widget;
    this->displaying_widget->set_displayed_object(this);
}

void AbstractModelObject::update_status(ControlledObjectStatus new_status)
{
    if (this->status != new_status)
        this->set_change_flag();
    else
        this->clear_change_flag();
    this->status = new_status;
}

ControlledObjectStatus AbstractModelObject::get_status()
{
    return this->status;
}

bool AbstractModelObject::has_changed()
{
    return this->has_changed_flag;
}

void AbstractModelObject::set_change_flag()
{
    this->has_changed_flag = true;
}

void AbstractModelObject::clear_change_flag()
{
    this->has_changed_flag = false;
}


AbstractControlledValue::AbstractControlledValue(int min_value, int max_value, bool wrap, int increment)
    : AbstractModelObject()
{
    this->value = 0;
    this->min_value = min_value;
    this->max_value = max_value;
    this->increment = increment;
    this->wrap = wrap;
}

AbstractControlledValue::~AbstractControlledValue()
{
}

int AbstractControlledValue::get_min_value()
{
    return min_value;
}

void AbstractControlledValue::set_min_value(int value)
{
    this->min_value = value;
}

int AbstractControlledValue::get_max_value()
{
    return max_value;
}

void AbstractControlledValue::set_max_value(int value)
{
    this->max_value = value;
}

int AbstractControlledValue::get_value()
{
    return value;
}

void AbstractControlledValue::increment_value()
{
    value += increment;
    if ((wrap) and (value > max_value))
        value = min_value;
    this->value = std::min(max_value, std::max(min_value, value));
    this->set_change_flag();
}

void AbstractControlledValue::decrement_value()
{
    value -= increment;
    if ((wrap) and (value < min_value))
        value = max_value;
    this->value = std::min(max_value, std::max(min_value, value));
    this->set_change_flag();
}

void AbstractControlledValue::set_value_clipped(int new_value)
{
    this->value = std::min(max_value, std::max(min_value, new_value));
    this->set_change_flag();
}

void AbstractControlledValue::process_control_event(ControlEvent event)
{
    switch (event)
    {
    case ControlEvent::INCREMENT:
        increment_value();
        break;
    case ControlEvent::DECREMENT:
        decrement_value();
        break;

    default:
        break;
    }
}

AbstractController::AbstractController()
{
}

AbstractController::~AbstractController()
{
}

void AbstractController::activate_controlled_object(AbstractModelObject *_controled_object)
{
    current_controlled_object->update_status(ControlledObjectStatus::HAS_FOCUS);
    this->current_controlled_object = _controled_object;
    current_controlled_object->update_status(ControlledObjectStatus::IS_ACTIVE);

}

void AbstractController::set_focus_on_controlled_object(AbstractModelObject *_controled_object)
{
    this->current_controlled_object = _controled_object;
    current_controlled_object->update_status(ControlledObjectStatus::HAS_FOCUS);
}

AbstractModelObject *AbstractController::get_current_controlled_object()
{
    return current_controlled_object;
}
