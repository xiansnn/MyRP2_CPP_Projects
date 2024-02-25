#if !defined(WIDGET_TEXT_H)
#define WIDGET_TEXT_H

#include "framebuffer.h"
#include "ui_mvc.h"
#include "pico/stdlib.h"

class WText : public Framebuffer , public UI_Widget
{
private:
    /* data */
public:
    WText(uint8_t id, size_t width, size_t height, uint8_t anchor_x=0,uint8_t anchor_y=0,
              Framebuffer_format format = Framebuffer_format::MONO_VLSB, 
              config_framebuffer_text_t txt_cnf = {.font = font_8x8});
    ~WText();
};





#endif // WIDGET_TEXT_H
