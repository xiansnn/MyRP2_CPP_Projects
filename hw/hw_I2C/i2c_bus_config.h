#if !defined(I2C_BUS_CONFIG_H)
#define I2C_BUS_CONFIG_H

//-----------------------------------------------------------------------------
//                             I2C bus config
//-----------------------------------------------------------------------------

#define I2C_STANDARD_MODE  100*1000  // <= 100kb/s
#define I2C_FAST_MODE      400*1000  // <= 400kb/s
#define I2C_FAST_MODE_PLUS 1000*1000 // <= 1Mb/s

#define I2C_SLAVE_DEFAULT_MAX_MEMORY_SIZE 256 // default value used by context_t

#define I2C_0_BAUD_RATE I2C_STANDARD_MODE
#define I2C_0_SDA_PIN 8 // choice in [0, (4), 8, 12, 16, 20]   (default)
#define I2C_0_SCL_PIN 9 // choice in [1, (5), 9, 13, 17, 21]   (default)
#define I2C_0_SLAVE_ADDR 0x55 // in case I2C0 is slave (default 0x55)
#define I2C_0_SLAVE_MAX_MEMORY_SIZE 256 // in case I2C0 is slave

#define I2C_1_BAUD_RATE I2C_STANDARD_MODE
#define I2C_1_SDA_PIN 6 // choice in [2, 6, 10, 14, 18, 26]
#define I2C_1_SCL_PIN 7 // choice in [3, 7, 11, 15, 19, 27]
#define I2C_1_SLAVE_ADDR 0x15 // in case I2C0 is slave (default 0x55)
#define I2C_1_SLAVE_MAX_MEMORY_SIZE 256 // in case I2C0 is slave


#endif // I2C_BUS_CONFIG_H



