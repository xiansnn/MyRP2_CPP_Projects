#if !defined(FOCUS_MANAGER_H)
#define FOCUS_MANAGER_H

#include "framebuffer.h"
#include "controlled_value.h"
#include "ky_040.h"

#include <string>
#include <map>

typedef struct focus_bind
{
    Framebuffer* framebuffer;
    ControlledValue* controlled_value;
    KY040Encoder* incremental_encoder;
}focus_bind_t;


class FocusManager
{
private:
public:
    std::map<std::string,focus_bind_t*> focus_bind_map;
    FocusManager();
    ~FocusManager();
    void add_focus_bind(std::string ID, focus_bind_t* bind);
};





#endif // FOCUS_MANAGER_H
