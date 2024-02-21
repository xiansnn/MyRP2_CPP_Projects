#if !defined(FOCUS_MANAGER_H)
#define FOCUS_MANAGER_H

#include <vector>
#include "ui_mvc.h"

class FocusManager : public UI_ControlledObject
{
private:
    uint8_t id;
    uint16_t focus_index;
    std::vector<UI_ControlledObject *> controlled_objects;
    UI_ControlledObject *current_focus;

public:
    FocusManager(uint8_t id);
    ~FocusManager();

    void add_controlled_object(UI_ControlledObject *cntrl_obj);
    UI_ControlledObject *update_current_focus();
    UI_ControlledObject *get_current_focus();
    void update_current_focus(UI_Controller *controller);
    void reset_focus();
    void process_control_event(ControlEvent event);

    void on_push();
    void on_long_push();
    void on_short_release();
    void on_long_release();
    void increment();
    void decrement();
};

#endif // FOCUS_MANAGER_H
