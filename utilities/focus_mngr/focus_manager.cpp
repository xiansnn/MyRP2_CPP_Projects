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

UI_ControlledObject *FocusManager::get_active_controlled_object()
{
    return controlled_objects[value];
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

void FocusManager::process_control_event(ControlEvent event)
{
    switch (event)
    {
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
}
