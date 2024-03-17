#if !defined(MSGEQ7_H)
#define MSGEQ7_H

#include "pico/stdlib.h"
#include <vector>
#include "hardware/adc.h"
#include <array>

class MSGEQ7
{
private:
    uint strobe_gpio;
    uint reset_gpio;
    uint signal_out;
    std::array<uint16_t,7> band_results;
public:
    MSGEQ7(uint strobe_gpio, uint reset_gpio,uint signal_out);
    ~MSGEQ7();

    std::array<uint16_t,7> get_spectrum();

};




#endif // MSGEQ7_H
