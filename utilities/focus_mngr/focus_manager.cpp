#include "focus_manager.h"

FocusManager::FocusManager(uint8_t id) : UI_ControlledObject(id)
{
    set_value(0);
}

FocusManager::~FocusManager()
{
}

void FocusManager::add_controlled_object(UI_ControlledObject *cntrl_obj)
{
    this->controlled_objects.push_back(cntrl_obj);
    this->max_value = controlled_objects.size() - 1;
}

// UI_ControlledObject *FocusManager::update_current_focus()
// {
//     current_focus = controlled_objects[focus_index];
//     return current_focus;
// }

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
    value++;
    if (value > max_value)
        value = min_value;
    value = std::min(max_value, std::max(min_value, value));
    has_changed = true;
}

void FocusManager::decrement()
{
    value--;
    if (value < min_value)
        value = max_value;
    value = std::min(max_value, std::max(min_value, value));
    has_changed = true;
}

// UI_ControlledObject *FocusManager::process_control_event(ControlEvent event)
// {

//     switch (event)
//     {
//     case ControlEvent::NOOP:
//         break;
//     case ControlEvent::PUSH:
//         break;
//     case ControlEvent::LONG_PUSH:
//         current_focus->reset();
//         break;
//     case ControlEvent::RELEASED_AFTER_SHORT_TIME:
//         if (current_focus->id == FOCUS_MANAGER_ID)
//             // current_focus = this->update_current_focus();
//             current_focus = controlled_objects[focus_index];
//         else
//             current_focus = this;

//         this->current_controller->set_active_controlled_object(current_focus);
//         this->current_widget->set_active_displayed_object(current_focus);
//         this->current_widget->draw();

//         break;
//     case ControlEvent::RELEASED_AFTER_LONG_TIME:
//         break;
//     case ControlEvent::INCREMENT:
//         value++;
//         if (value > max_value)
//             value = min_value;
//         value = std::min(max_value, std::max(min_value, value));
//         focus_index = value;
//         has_changed = true;
//         break;
//     case ControlEvent::DECREMENT:
//         value--;
//         if (value < min_value)
//             value = max_value;
//         value = std::min(max_value, std::max(min_value, value));
//         focus_index = value;
//         has_changed = true;
//         break;

//     default:
//         break;
//     }
//     return current_focus;
// }

void FocusManager::update_current_focus(UI_Controller *controller)
{
    ControlEvent sw_event = controller->get_control_event();
    switch (sw_event)
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
            // current_focus = this->update_current_focus();
            current_focus = controlled_objects[focus_index];
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
        has_changed = true;
        break;
    case ControlEvent::DECREMENT:
        value--;
        if (value < min_value)
            value = max_value;
        value = std::min(max_value, std::max(min_value, value));
        has_changed = true;
        /* code */
        break;

    default:
        break;
    }
    current_focus = controlled_objects[focus_index];
}

UI_ControlledObject *FocusManager::get_current_focus()
{
    return current_focus;
}
