#if !defined(WIDGETS_BAR_H)
#define WIDGETS_BAR_H

#include "framebuffer.h"
#include "controlled_value.h"
#include "ui_mvc.h"

typedef struct config_bar_widget
{
    size_t width{128};
    size_t height{8};
    bool with_border{true};
    bool with_label {false};
    const unsigned char *font{nullptr};
} config_bar_widget_t;

class Bar : public Framebuffer, public UI_Widget
{
private:
    ControlledValue* cntrl_value;
    config_bar_widget_t config{};
    size_t label_value_max_width;
    uint8_t px_max;
    uint8_t px_min;
    // uint8_t px;
    float level_coef;
    int level_offset;

    uint8_t convert_level_value_to_px(int level);

public:
    Bar(uint8_t id, ControlledValue* cntrl_value,  config_bar_widget_t bar_config ={});
    ~Bar();
    void draw();
    void draw_border();
    void draw_level_value(int value);
};

#endif // WIDGETS_BAR_H
