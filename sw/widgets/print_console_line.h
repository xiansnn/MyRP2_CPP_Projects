#if !defined(PRINT_CONSOLE_LINE)
#define PRINT_CONSOLE_LINE

#include "ui_mvc.h"
#include "controlled_value.h"



class DisplayControlledValueOnTerminal : public UI_Widget
{
private:
    float slope;
    float offset;
    uint8_t max_line_width = 21;

public:
    DisplayControlledValueOnTerminal(uint8_t id);
    ~DisplayControlledValueOnTerminal();
    void draw();
    UI_ControlledObject* set_active_displayed_object(UI_ControlledObject *val);
};


#endif // PRINT_CONSOLE_LINE
