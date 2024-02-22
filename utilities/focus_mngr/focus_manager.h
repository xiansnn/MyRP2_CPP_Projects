#if !defined(FOCUS_MANAGER_H)
#define FOCUS_MANAGER_H

#include <vector>
#include "ui_mvc.h"
#include "ui_focus_manager.h"
#include "switch_button.h"

class FocusManager : public UI_FocusManager
{
private:
    
public:
    FocusManager();
    ~FocusManager();
    
    void process_control_event(SwitchButton* controller);
    void process_control_event(ControlEvent event);

};

#endif // FOCUS_MANAGER_H
