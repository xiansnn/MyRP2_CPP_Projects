#if !defined(WIDGETS_BAR_H)
#define WIDGETS_BAR_H

#include "framebuffer.h"
#include "controlled_value.h"
#include "ui_mvc.h"

typedef struct config_widget
{
    size_t width{128};
    size_t height{8};
    bool with_border{true};
    bool with_label{false};
    const unsigned char *font{nullptr};
} config_widget_t;

class W_Bar : public UI_Widget
{
private:
    size_t label_value_max_width;
    uint8_t px_max;
    uint8_t px_min;
    float level_coef;
    int level_offset;

    uint8_t convert_level_value_to_px(int level);

protected:
    config_widget_t config{};

public:
    W_Bar(uint8_t id, ControlledValue *cntrl_value, uint8_t anchor_x, uint8_t anchor_y, config_widget_t bar_config = {});
    ~W_Bar();
    void draw();
    void draw_border();
    void draw_level_value(int value);
};

#endif // WIDGETS_BAR_H
