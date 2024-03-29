#if !defined(CNTRL_VALUE_H)
#define CNTRL_VALUE_H

#include "ui_mvc.h"

class ControlledValue : public UI_ControlledObject
{
private:
    bool wrap;
    UI_Controller* controller;
    UI_Widget* widget;

public:
    ControlledValue(uint8_t id, int min_value, int max_value, int increment = 1, bool wrap = false) ;
    ~ControlledValue();

    void process_control_event(ControlEvent event);

};

#endif // CNTRL_VALUE_H
