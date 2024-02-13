#if !defined(WIDGETS_BAR_H)
#define WIDGETS_BAR_H

#include "framebuffer.h"

typedef struct config_bar_widget
{
    int level_max{10};
    int level_min{0};
    size_t width{128};
    size_t height{8};
    bool draw_border{true};
    bool draw_value {false};
    const unsigned char *font{nullptr};
} config_bar_widget_t;

class Bar : public Framebuffer
{
private:
    size_t value_max_width;
    uint8_t px_max;
    uint8_t px_min;
    float level_coef;
    int level_offset;
    config_bar_widget_t config{};

    uint8_t convert_level_to_px(int level);

public:
    Bar(config_bar_widget_t bar_config);
    ~Bar();

    int level{0};
    uint8_t px;

    void init(config_bar_widget_t conf);

    // void reset_px();
    // void increment_level();
    // void decrement_level();
    void draw();
};

#endif // WIDGETS_BAR_H
