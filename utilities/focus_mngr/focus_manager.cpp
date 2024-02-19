#include "focus_manager.h"

FocusManager::FocusManager(uint8_t id) : UI_ControlledObject(id)
{
    set_value(0);
}

FocusManager::~FocusManager()
{
}

// UI_ControlledObject *FocusManager::next_focus()
// {
//     current_index++;
//     current_index %= max_value;
//     return controlled_objects[current_index];
// }

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

// void FocusManager::reset()
// {
//     current_index = 0;
// }

// int FocusManager::get_value()
// {
//     return current_index;
// }

// int FocusManager::get_min_value()
// {
//     return 0;
// }

// int FocusManager::get_max_value()
// {
//     return this->controlled_objects.size();
// }

// void FocusManager::set_value(uint8_t new_value)
// {
//     current_index = new_value;
// }
