#include "focus_manager.h"

FocusManager::FocusManager() : UI_ControlledObject(FOCUS_MANAGER_ID)
{
    min_value=1;
    focus_index = 0;
    add_controlled_object(this);
    set_value(0);
}

FocusManager::~FocusManager()
{
}

void FocusManager::add_controlled_object(UI_ControlledObject *cntrl_obj)
{
    this->controlled_objects.push_back(cntrl_obj);
    this->max_value = controlled_objects.size()-1;
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
    focus_index = value;
    has_changed = true;
}

void FocusManager::decrement()
{
    value--;
    if (value < min_value)
        value = max_value;
    value = std::min(max_value, std::max(min_value, value));
    focus_index = value;
    has_changed = true;
}
