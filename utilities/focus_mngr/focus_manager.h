#if !defined(FOCUS_MANAGER_H)
#define FOCUS_MANAGER_H

#include <vector>
#include "ui_mvc.h"

class FocusManager : public UI_ControlledObject
{
private:
    uint8_t focus_index;
    std::vector<UI_ControlledObject *> controlled_objects;

public:
    FocusManager();
    ~FocusManager();

    void add_controlled_object(UI_ControlledObject *cntrl_obj);
    UI_ControlledObject *get_active_controlled_object();

    void process_control_event(ControlEvent event);

};

#endif // FOCUS_MANAGER_H
