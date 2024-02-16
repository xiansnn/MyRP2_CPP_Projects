#include "focus_manager.h"


FocusManager::FocusManager(/* args */)
{
}

FocusManager::~FocusManager()
{
}

void FocusManager::add_focus_bind(std::string ID, focus_bind_t* bind)
{
    this->focus_bind_map[ID]=bind;
}
