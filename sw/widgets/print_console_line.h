#if !defined(PRINT_CONSOLE_LINE)
#define PRINT_CONSOLE_LINE

#include "ui_mvc.h"
#include "controlled_value.h"

class W_DisplayControlledValueOnTerminal : public UI_Widget
{
private:
    float slope;
    float offset;
    uint8_t max_line_width = 21;

public:
    W_DisplayControlledValueOnTerminal(uint8_t id, size_t width, size_t height, uint8_t anchor_x = 0, uint8_t anchor_y = 0,
              Framebuffer_format format = Framebuffer_format::MONO_VLSB,
              config_framebuffer_text_t txt_cnf = {.font = font_8x8});
    ~W_DisplayControlledValueOnTerminal();
    void draw();
    UI_ControlledObject *set_active_displayed_object(UI_ControlledObject *val);
};

#endif // PRINT_CONSOLE_LINE
