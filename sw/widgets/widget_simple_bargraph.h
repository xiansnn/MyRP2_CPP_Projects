#if !defined(WIDGET_SIMPLE_BARGRAPH_H)
#define WIDGET_SIMPLE_BARGRAPH_H

#include "ui_mvc.h"

typedef struct config_simple_bargraph_widget
{
    // config UI_Widget
    uint8_t bargraph_anchor_x;
    uint8_t bargraph_anchor_y;
    size_t bargraph_width;
    size_t bargraph_height;
    bool with_border{false};
    uint8_t bargraph_bin_number;
    uint8_t bargraph_bin_spacing{1};
    uint8_t border_width{1};

    // config Framebuffer
    Framebuffer_format format{Framebuffer_format::MONO_VLSB};
    config_framebuffer_text_t txt_cnf{.font = font_8x8};
} config_simple_bargraph_widget_t;

//-----class BargraphDisplayedObject
class BargraphDisplayedObject
{
private:
public:
    BargraphDisplayedObject(int min_value = 0, int max_value = 10);
    ~BargraphDisplayedObject();
    std::vector<int> values;
    int min_value;
    int max_value;
};

//-----class W_SimpleHBargraph : public AbstractWidget
class W_SimpleHBargraph : public AbstractWidget
{
private:
    BargraphDisplayedObject *displayed_values;

protected:
    uint8_t bargraph_bin_spacing;
    uint8_t bargraph_bin_flag_width;

    uint8_t bar_height;

    uint8_t px_max;
    uint8_t px_min;
    uint8_t bar_width;
    float level_coef;
    int level_offset;

    uint8_t convert_level_value_to_px(int level);
    void draw();
    void draw_bar(uint8_t bin_number);

public:
    W_SimpleHBargraph(UI_DisplayDevice *display_screen, BargraphDisplayedObject *displayed_values, config_simple_bargraph_widget_t cnf_bargraph);
    ~W_SimpleHBargraph();
    uint8_t bargraph_bin_number;
};

#endif // WIDGET_SIMPLE_BARGRAPH_H
