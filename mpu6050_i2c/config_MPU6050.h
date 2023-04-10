//---I2C bus config---
//--------------------
#define I2C_BUS i2c0
// #define I2C_BUS i2c1
#define I2C_SPEED 100000 // I2C STANDARD-MODE
// #define I2C_SPEED 400000 // I2C FAST-MODE
#define I2C_SDA 8 // gpio pin on MakerFab Primer Board
#define I2C_SCL 9 // gpio pin on MakerFab Primer Board

//---define MPU config---
//-----------------------
#define MPU_ADDR 0x68  // assuming AD0 pin is low
// define MPU sensors sample rate
#define SAMPLE_RATE 50.0 // in Hz
#define SMPLRT_DIV_RA 0x19
// config FSYNC and DLPF config
#define CONFIG_RA 0x1A
#define CONFIG_FSYNC_DLPF CONFIG_FSYNC_DISABLED | CONFIG_DLPF_DISABLED
#define CONFIG_FSYNC_DISABLED 0x0
#define CONFIG_DLPF_DISABLED 0x0
#define GYRO_OUT_RATE 8000 // assuming DLPF is disabled,//
// config Gyro
#define GYRO_CONFIG_RA 0x1B
// config Accelerator
#define ACCEL_CONFIG_RA 0x1C// define MPU clock source
// cofig FIFO
#define FIFO_EN_RA 0x23 // a 1024-byte FIFO
// #define FIFO_EN_VALUE FIFO_EN_TEMP_FIFO_EN | ALL_GYRO | FIFO_EN_ACCEL_FIFO_EN
#define FIFO_EN_VALUE FIFO_EN_TEMP_FIFO_EN | FIFO_EN_ACCEL_FIFO_EN | ALL_GYRO
#define ALL_GYRO FIFO_EN_XG_FIFO_EN | FIFO_EN_YG_FIFO_EN | FIFO_EN_ZG_FIFO_EN
#define FIFO_EN_TEMP_FIFO_EN 0x80
#define FIFO_EN_XG_FIFO_EN 0x40
#define FIFO_EN_YG_FIFO_EN 0x20
#define FIFO_EN_ZG_FIFO_EN 0x10
#define FIFO_EN_ACCEL_FIFO_EN 0x08
#define FIFO_EN_SLV2_FIFO_EN 0x04
#define FIFO_EN_SLV1_FIFO_EN 0x02
#define FIFO_EN_SLV0_FIFO_EN 0x01
// config Interrupt
#define INT_STATUS_RA 0x3A
// measures registers
#define ACCEL_XOUT_H_RA 0x3B // the first data serves as a base for all MPU measures
// config FIFO, I2C, signal path
#define USER_CTRL_RA 0x6A
#define USER_CTRL_FIFO_EN 0x40
#define USER_CTRL_I2C_MST_EN 0x20
#define USER_CTRL_I2C_IF_DIS 0x10 // MPU6000 only
#define USER_CTRL_FIFO_RESET 0x04
#define USER_CTRL_I2C_MST_RESET 0x02
#define USER_CTRL_SIG_COND_RESET 0x01
// config power, reset, sleep mode
#define PWR_MGMT_1_RA 0x6B
#define PWR_MGMT_1_CLKSEL PWR_MGMT_1_CLKSEL_Z_PLL
#define PWR_MGMT_1_CLKSEL_INTERNAL 0x00
#define PWR_MGMT_1_CLKSEL_X_PLL 0x01
#define PWR_MGMT_1_CLKSEL_Y_PLL 0x02
#define PWR_MGMT_1_CLKSEL_Z_PLL 0x03
// FIFO count register
#define FIFO_COUNT_H_RA 0x72
// measures FIFO entry
#define FIFO_R_W_RA 0x74
// MPU address default register
#define WHO_AM_I_RA 0x75

