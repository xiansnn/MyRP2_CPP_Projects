#if !defined(I2C_BUS_CONFIG_H)
#define I2C_BUS_CONFIG_H

//-----------------------------------------------------------------------------
//                             I2C bus config
//-----------------------------------------------------------------------------

#define I2C_STANDARD_MODE 100*1000  // 100kb/s
#define I2C_FAST_MODE     400*1000  // 400kb/s

#define I2C_MASTER i2c0
#define I2C_MASTER_BAUD_RATE I2C_STANDARD_MODE
#define I2C_MASTER_SDA_PIN 8 // gpio pin on MakerFab Primer Board
#define I2C_MASTER_SCL_PIN 9 // gpio pin on MakerFab Primer Board

#define I2C_SLAVE i2c1
#define I2C_SLAVE_BAUD_RATE I2C_STANDARD_MODE
#define I2C_SLAVE_SDA_PIN 6 // gpio pin on MakerFab Primer Board
#define I2C_SLAVE_SCL_PIN 7 // gpio pin on MakerFab Primer Board
#define I2C_SLAVE_ADDR 0x65
#define I2C_SLAVE_MAX_MEMORY_SIZE 256


#endif // I2C_BUS_CONFIG_H



