#include "focus_manager.h"
#include <string>

FocusManager::FocusManager() : UI_ControlledObject(FOCUS_MANAGER_ID)
{
    this->focus_index = 0;
    this->min_value = 1;
    this->add_controlled_object(this);
    this->current_focus = this->controlled_objects[focus_index];
}

FocusManager::~FocusManager()
{
}

void FocusManager::add_controlled_object(UI_ControlledObject *cntrl_obj)
{
    this->controlled_objects.push_back(cntrl_obj);
    this->max_value = controlled_objects.size() - 1;
}

UI_ControlledObject *FocusManager::update_current_focus()
{
    current_focus = controlled_objects[focus_index];
    return current_focus;
}

void FocusManager::reset_focus()
{
    this->focus_index = 0;
    current_focus = controlled_objects[focus_index];
}

void FocusManager::on_push()
{
}

void FocusManager::on_long_push()
{
}

void FocusManager::on_short_release()
{
}

void FocusManager::on_long_release()
{
}

void FocusManager::increment()
{
}

void FocusManager::decrement()
{
}

UI_ControlledObject *FocusManager::process_control_event(ControlEvent event)
{

    switch (event)
    {
    case ControlEvent::NOOP:
        break;
    case ControlEvent::PUSH:
        break;
    case ControlEvent::LONG_PUSH:
        current_focus->reset();
        break;
    case ControlEvent::RELEASED_AFTER_SHORT_TIME:
        if (current_focus->id == FOCUS_MANAGER_ID)
            current_focus = this->update_current_focus();
        else
            current_focus = this;

        this->current_controller->set_active_controlled_object(current_focus);
        this->current_widget->set_active_displayed_object(current_focus);
        this->current_widget->draw();

        break;
    case ControlEvent::RELEASED_AFTER_LONG_TIME:
        break;
    case ControlEvent::INCREMENT:
        value++;
        if (value > max_value)
            value = min_value;
        value = std::min(max_value, std::max(min_value, value));
        focus_index = value;
        has_changed = true;
        break;
    case ControlEvent::DECREMENT:
        value--;
        if (value < min_value)
            value = max_value;
        value = std::min(max_value, std::max(min_value, value));
        focus_index = value;
        has_changed = true;
        break;

    default:
        break;
    }
    return current_focus;
}

void FocusManager::process_focus(UI_Controller *controller, UI_ControlledObject *controlled_object)
{
    ControlEvent sw_event = controller->get_control_event();//  .process_sample_event();
    controlled_object = process_control_event(sw_event);
}
