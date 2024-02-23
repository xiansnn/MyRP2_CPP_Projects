#if !defined(MB_FOCUS_MANAGER_H)
#define MB_FOCUS_MANAGER_H

#include "ui_focus_manager.h"
#include "switch_button.h"

class MB_FocusManager : public UI_FocusManager
{
private:
    /* data */
public:
    MB_FocusManager(/* args */);
    ~MB_FocusManager();

    void process_control_event(SwitchButton* controller);
    void process_control_event(ControlEvent event);
};

#endif // MB_FOCUS_MANAGER_H
