#include"widget_text.h"

WText::WText(uint8_t id, size_t width, size_t height, Framebuffer_format format, config_framebuffer_text_t txt_cnf)
: Framebuffer(width, height, format, txt_cnf) , UI_Widget(id)
{
}

WText::~WText()
{
}

// void WText::draw()
// {
//     clear_text_buffer();
//     sprintf(text_buffer, "%5.1f MHz", (float)val0.get_value() / 10);
//     print_text();
// }
