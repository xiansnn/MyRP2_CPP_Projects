#if !defined(WIDGETS_H)
#define WIDGETS_H

#include "framebuffer.h"

struct bar_widget_config_t
{
    int cursor_max{10};
    int cursor_min{0};
    size_t width{128};
    size_t height{8};
    bool border{true};
};

class Bar : public Framebuffer
{
private:
    bar_widget_config_t bar_config{};
    uint8_t bar_zero;
    int bar_cursor_coef;
    size_t bar_size{0};

    

public:
    Bar(bar_widget_config_t bar_config);
    ~Bar();

    int cursor{0};
    void reset_cursor();
    void increment_cursor();
    void decrement_cursor();
    void draw();
};

#endif // WIDGETS_H
