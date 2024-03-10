#if !defined(WIDGET_BARGRAPH_H)
#define WIDGET_BARGRAPH_H

#include "rotary_encoder.h"
#include "ui_mvc.h"
#include "widget_bar.h"
#include "controlled_value.h"
#include <array>

#define BARGRAPH_BIN_NUMBER 7
#define BARGRAPH_BIN_SPACING 1 // pixel

//-----class BargraphDisplayedObject
class BargraphDisplayedObject
{
private:


public:
    BargraphDisplayedObject(uint8_t id, int min_value = 0, int max_value = 10);
    ~BargraphDisplayedObject();
    std::array<uint8_t, BARGRAPH_BIN_NUMBER> values;
    int min_value;
    int max_value;
    bool status_has_changed{true};
    void set_value_clipped(uint8_t index, int new_value);
};

//---------class W_HBargraph : public UI_Widget
class W_HBargraph : public UI_Widget , public UI_ControlledObject
{
private:
    UI_DisplayDevice *screen_framebuffer;
    BargraphDisplayedObject *displayed_values;
    config_widget_t config;
    uint8_t px_max;
    uint8_t px_min;
    uint8_t bar_height;
    float level_coef;
    int level_offset;

    uint8_t convert_level_value_to_px(int level);
    void draw();
    void draw_bar(uint8_t bin_number, uint8_t x, uint8_t y, size_t w, size_t h, bool with_border );
    void set_value_clipped(int new_value);

public:
    W_HBargraph(uint8_t id, UI_DisplayDevice *screen, uint8_t anchor_x, uint8_t anchor_y,
                BargraphDisplayedObject *displayed_values, config_widget_t cnf_bar,
                Framebuffer_format format = Framebuffer_format::MONO_VLSB,
                config_framebuffer_text_t txt_cnf = {.font = font_5x8});
    ~W_HBargraph();

    void process_control_event(ControlEvent event);
    void refresh();
};

#endif // WIDGET_BARGRAPH_H
