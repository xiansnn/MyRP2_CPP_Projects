#if !defined(FOCUS_MANAGER_H)
#define FOCUS_MANAGER_H

#include <vector>
#include "ui_mvc.h"

class FocusManager : public UI_ControlledObject
{
private:
    uint8_t id;
    // int current_index = 0;
    std::vector<UI_ControlledObject *> controlled_objects;

public:
    FocusManager(uint8_t id);
    ~FocusManager();

    // UI_ControlledObject *next_focus();
    void add_controlled_object(UI_ControlledObject *cntrl_obj);
    UI_ControlledObject *get_active_controlled_object();
    void on_push();
    void on_long_push();
    void on_short_release();
    void on_long_release();
    void increment();
    void decrement();
    // void reset();
    // int get_value();
    // int get_min_value();
    // int get_max_value();
    // void set_value(uint8_t new_value);
};

#endif // FOCUS_MANAGER_H
