#if !defined(SSD1306_H)
#define SSD1306_H

#include "commands_SSD1306.h"
#include "pico/stdlib.h"
#include "hw_I2C/hw_i2c.h"

typedef struct config_SSD1306
{
    uint8_t SSD1306_ADDR = 0x3C;
    uint8_t MUX_RATIO = SSD1306_SET_MUX_RATIO;

} config_SSD1306_t;

class SSD1306
{
private:
    hw_I2C_master *i2c_master;
    config_SSD1306_t config;
public:
    SSD1306(hw_I2C_master *master, config_SSD1306_t default_config);
    void init_display();
};


#endif // SSD1306_H


// SW initialisation flow
//, 3Fh (= 63d = 64MUX)
// Set Display Offset
//   SSD1306_SET_DISP_OFFSET, 00h
// Set Display Start Line
//   SSD1306_SET_DISP_START_LINE (0)
// Set Segment re-map
//   SSD1306_SET_SEG_NO_REMAP  |  SSD1306_SET_SEG_REMAP
// Set COM Output Scan Direction
//   SSD1306_SET_COM_OUT_DIR  |  SSD1306_SET_COM_OUT_DIR_REMAP
// Set COM Pins hardware configuration
//   SSD1306_SET_COM_PIN_CFG, 02
// Set Contrast Control
//   SSD1306_SET_CONTRAST, 7Fh
// Disable Entire Display On
//   SSD1306_SET_RAM_DISPLAY
// Set Normal Display
//   SSD1306_SET_NORM_DISP
// Set Osc Frequency
//   SSD1306_SET_DISP_CLK_DIV, 80h
// Enable charge pump regulator
//   SSD1306_SET_CHARGE_PUMP, 14h
// Display On
//   SSD1306_SET_DISPLAY_NORMAL_ON