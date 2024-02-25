#include"widget_text.h"

WText::WText(uint8_t id, size_t width, size_t height, uint8_t anchor_x,uint8_t anchor_y,
Framebuffer_format format, config_framebuffer_text_t txt_cnf)
: Framebuffer(width, height, format, txt_cnf) , UI_Widget(id)
{
}

WText::~WText()
{
}

