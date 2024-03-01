#if !defined(WIDGET_BARGRAPH_H)
#define WIDGET_BARGRAPH_H

#include "ui_mvc.h"


// #define BARGRAPH_NUMBER 7


class W_Bargraph : public UI_Widget
{
private:
    
public:
    W_Bargraph(uint8_t id, size_t width, size_t height, uint8_t anchor_x = 0, uint8_t anchor_y = 0,
              Framebuffer_format format = Framebuffer_format::MONO_VLSB,
              config_framebuffer_text_t txt_cnf = {.font = font_8x8});
    ~W_Bargraph();
};





#endif // WIDGET_BARGRAPH_H
