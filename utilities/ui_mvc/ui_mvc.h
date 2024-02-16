#if !defined(UI_MVC_H)
#define UI_MVC_H

#include <map>
#include "pico/stdlib.h"

class UI_ControlledObject;
class UI_Widget;

class UI_Controller
{
private:
    /* data */
public:
    uint8_t id;
    UI_ControlledObject* controlled_object;

    UI_Controller(/* args */);
    ~UI_Controller();

    void set_controlled_object(UI_ControlledObject* cntrl_obj);

};


class UI_ControlledObject
{
private:
    /* data */
public:
    uint8_t id;
    std::map<uint8_t,UI_Controller*> controllers;
    std::map<uint8_t,UI_Widget*> widgets;

    UI_ControlledObject(/* args */);
    ~UI_ControlledObject();

    void add_widget(UI_Widget* widget);
    void add_controller(UI_Controller* controller);


};


class UI_Widget
{
private:
    /* data */
public:
    uint8_t id;
    std::map<uint8_t,UI_ControlledObject*> displayed_objects;

    UI_Widget(/* args */);
    ~UI_Widget();

    void add_displayed_object(UI_ControlledObject* displayed_object);
};



#endif // UI_MVC_H
