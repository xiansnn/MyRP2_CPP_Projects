#if !defined(FOCUS_MANAGER_H)
#define FOCUS_MANAGER_H

#include <vector>
#include "ui_mvc.h"

class FocusManager : public UI_ControlledObject
{
private:
    uint8_t id;
    std::vector<UI_ControlledObject *> controlled_objects;

public:
    FocusManager(uint8_t id);
    ~FocusManager();

    void add_controlled_object(UI_ControlledObject *cntrl_obj);
    // UI_ControlledObject *update_current_focus();
    void update_current_focus(UI_Controller* controller);
    UI_ControlledObject* get_current_focus();
    void reset_focus();
    void on_push();
    void on_long_push();
    void on_short_release();
    void on_long_release();
    void increment();
    void decrement();
    // UI_ControlledObject *process_control_event(ControlEvent event);
};

#endif // FOCUS_MANAGER_H
