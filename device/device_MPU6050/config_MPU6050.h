#if !defined(CONFIG_MPU6050_H)
#define CONFIG_MPU6050_H

#include "register_address_MPU6050.h"

//-----------------------------------------------------------------------------
//                             I2C bus config
//-----------------------------------------------------------------------------
// set I2C channel 0 or 1
#define I2C_BUS i2c0
// #define I2C_BUS i2c1
// set I2C speed Fast or Standard
#define I2C_SPEED I2C_STANDARD_MODE 
// set I2C SDA and SCL pin
#define I2C_SDA 8 // gpio pin on MakerFab Primer Board
#define I2C_SCL 9 // gpio pin on MakerFab Primer Board
#define MPU_ADDR 0x68    // assuming AD0 pin is low

//-----------------------------------------------------------------------------
//                               MPU config
//-----------------------------------------------------------------------------
#define SAMPLE_RATE 100.0 // set sensors sample rate in Hz

// ==== Register 26 0x1A– Configuration
#define CONFIG EXT_SYNC_SET | DLPF_CODE
// set Digital Low-Pass Filter bandwidth
#define DLPF_BW 50 // values in { 250, 200, 100, 50, 20, 10, 5}Hz
//                                   Accelero           |                       Gyro
#if (DLPF_BW == 250)   // BW = 260Hz | delai =  0.0ms   |   BW = 256Hz | delai =  0.98ms | gyro_sampling_rate = 8kHz
#define GYRO_OUT_RATE 8000
#define DLPF_CODE 0x00
#elif (DLPF_BW == 200) // BW = 184Hz | delai =  2.0ms   |   BW = 188Hz | delai =  1.90ms | gyro_sampling_rate = 1kHz
#define GYRO_OUT_RATE 1000
#define DLPF_CODE 0x01
#elif (DLPF_BW == 100) // BW =  94Hz | delai =  3.0ms   |   BW =  98Hz | delai =  2.80ms | gyro_sampling_rate = 1kHz
#define GYRO_OUT_RATE 1000
#define DLPF_CODE 0x02
#elif (DLPF_BW == 50)  // BW =  44Hz | delai =  4.9ms   |   BW =  42Hz | delai =  4.80ms | gyro_sampling_rate = 1kHz
#define GYRO_OUT_RATE 1000
#define DLPF_CODE 0x03
#elif (DLPF_BW == 20)  // BW =  21Hz | delai =  8.5ms   |   BW =  20Hz | delai =  8.30ms | gyro_sampling_rate = 1kHz
#define GYRO_OUT_RATE 1000
#define DLPF_CODE 0x04
#elif (DLPF_BW == 10)  // BW =  10Hz | delai = 13.8ms   |   BW =  10Hz | delai = 13.40ms | gyro_sampling_rate = 1kHz
#define GYRO_OUT_RATE 1000
#define DLPF_CODE 0x05
#elif (DLPF_BW == 5)   // BW =   5Hz | delai = 19.0ms   |   BW =   5Hz | delai = 18.60ms | gyro_sampling_rate = 1kHz
#define GYRO_OUT_RATE 1000
#define DLPF_CODE 0x00
#endif //
// set external FSYNC
#define EXT_SYNC_SET EXT_SYNC_SET_DISABLED
// values to be selected
#define  EXT_SYNC_SET_DISABLED     0x00 <<3 
#define  EXT_SYNC_SET_TEMP_OUT_L   0x01 <<3 
#define  EXT_SYNC_SET_GYRO_XOUT_L  0x02 <<3 
#define  EXT_SYNC_SET_GYRO_YOUT_L  0x03 <<3 
#define  EXT_SYNC_SET_GYRO_ZOUT_L  0x04 <<3 
#define  EXT_SYNC_SET_ACCEL_XOUT_L 0x05 <<3 
#define  EXT_SYNC_SET_ACCEL_YOUT_L 0x06 <<3 
#define  EXT_SYNC_SET_ACCEL_ZOUT_L 0x07 <<3
// end values

// ==== Register 27 0x1B– Gyroscope Configuration 
#define GYRO_FULL_SCALE_RANGE 250  // values in { 250, 500, 1000, 2000} DPS
#if (GYRO_FULL_SCALE_RANGE == 250)
#define GYRO_CONFIG 0x00 << 3
#elif (GYRO_FULL_SCALE_RANGE == 500)
#define GYRO_CONFIG 0x01 << 3
#elif (GYRO_FULL_SCALE_RANGE == 1000)
#define GYRO_CONFIG 0x02 << 3
#elif (GYRO_FULL_SCALE_RANGE == 2000)
#define GYRO_CONFIG 0x03 << 3
#endif

// ==== Register 28 0x1C– Accelerometer Configuration 
#define ACCEL_FULL_SCALE_RANGE 2  // values in { 2, 4, 8, 16} G
#if (ACCEL_FULL_SCALE_RANGE == 2)
#define ACCEL_CONFIG 0x00 << 3
#elif (ACCEL_FULL_SCALE_RANGE == 4)
#define ACCEL_CONFIG 0x01 << 3
#elif (ACCEL_FULL_SCALE_RANGE == 8)
#define ACCEL_CONFIG 0x02 << 3
#elif (ACCEL_FULL_SCALE_RANGE == 16)
#define ACCEL_CONFIG 0x03 << 3
#endif

// ==== Register 35 0x23– FIFO Enable : config which measures will be written into the FIFO
#define FIFO_SELECTED_SENSORS GYRO_FIFO_EN | ACCEL_FIFO_EN
// #define FIFO_SELECTED_SENSORS ACCEL_FIFO_EN
// values to be selected :
#define GYRO_FIFO_EN XG_FIFO_EN | YG_FIFO_EN | ZG_FIFO_EN
#define TEMP_FIFO_EN 0x80
#define XG_FIFO_EN 0x40
#define YG_FIFO_EN 0x20
#define ZG_FIFO_EN 0x10
#define ACCEL_FIFO_EN 0x08
#define SLV2_FIFO_EN 0x04
#define SLV1_FIFO_EN 0x02
#define SLV0_FIFO_EN 0x01
// end values

// ==== Register 55 0x37– INT Pin / Bypass Enable Configuration
#define INT_PIN_CFG INT_LEVEL | LATCH_INT_EN // Active LO, open drain, pulsed 50us, cleared by read INT status
// values to be selected :
#define INT_LEVEL 0x80    // INT pin active LOW
#define INT_OPEN 0x40     // INT pin open drain
#define LATCH_INT_EN 0x20 // INT pin held Hi until INT cleared
#define INT_RD_CLEAR 0x10 // INT pin cleared by any read
#define FSYNC_INT_LEVEL 0x08
#define FSYNC_INT_EN 0x04
#define I2C_BYPASS_EN 0x02
// end values

// ==== Register 56 0x38– Interrupt Enable
#define INT_ENABLE DATA_RDY_EN // INT each time a sensor register write occurs
// values to be selected :
#define FIFO_OFLOW_EN 0x10
#define I2C_MST_INT_EN 0x08
#define DATA_RDY_EN 0x01
// end values

// ==== Register 58 0x3A– read Interrupt Status
// values explaining reading
#define FIFO_OFLOW_INT 0x10
#define I2C_MST_INT 0x08
#define DATA_RDY_INT 0x01
// end values

// ==== Register 104 0x68– Signal Path Reset
#define SIGNAL_PATH_RESET 0x00 // default: no reset
// values to be selected
#define GYRO_RESET 0x04
#define ACCEL_RESET 0x02
#define TEMP_RESET 0x01
// end values

// ==== Register 106 0x6A– User Control
// config FIFO, I2C, signal path
#define FIFO_EN 0x40        // enables FIFO operations
#define I2C_MST_EN 0x20     // enables I2C master mode
#define I2C_IF_DIS 0x10     // MPU6000 only
#define FIFO_RESET 0x04     // reset and disables FIFO
#define I2C_MST_RESET 0x02  // reset I2C master while I2C_MST_EN = 0x00
#define SIG_COND_RESET 0x01 // reset all sensors processing and registers

// Register 107 0x6B– Power Management 1
#define PWR_MGMT_1 CLKSEL_Z_PLL
// values to be selected as CLKSEL
#define CLKSEL_INTERNAL 0x00
#define CLKSEL_X_PLL 0x01
#define CLKSEL_Y_PLL 0x02
#define CLKSEL_Z_PLL 0x03
// values to be selected as DEVICE_RESET
#define DEVICE_RESET 0x80
// end values

#endif // CONFIG_MPU6050_H