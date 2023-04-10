// #include <stdio.h>
// #include <string.h>
#include <map>
#include <vector>
#include "hardware/i2c.h"

// configuration of accelerometer
typedef struct g_scale
{
    uint8_t max_g_code;
    uint8_t max_g_value;
} g_scale_t;
enum ACCEL_FULL_SCALE_RANGE
{
    _2, // range +/- 2g
    _4, // range +/- 4g
    _8, // range +/- 8g
    _16 // range +/- 16g
};

// configuration of gyrometer
typedef struct gyro_scale
{
    uint8_t max_gyro_code;
    uint16_t max_gyro_value;
} gyro_scale_t;
enum GYRO_FULL_SCALE_RANGE
{
    _250,  // range +/- 250 degrees per second
    _500,  // range +/- 500 degrees per second
    _1000, // range +/- 1000 degrees per second
    _2000  // range +/- 2000 degrees per second
};

// raw data as they are captured by sensor
typedef struct RawData
{
    int16_t g_x;
    int16_t g_y;
    int16_t g_z;
    int16_t temp_out;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} RawData_t;

// measured sensor value after scale correction
typedef struct MPUData
{
    float g_x;
    float g_y;
    float g_z;
    float temp_out;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} MPUData_t;

// MPU 6050 device
class MPU6050
{
private:
    float acceleration_factor;
    float gyro_factor;
    float temperature_gain = (float)1 / 340;
    float temperature_offset = 36.53;
    float sample_rate;
    std::map<GYRO_FULL_SCALE_RANGE, gyro_scale_t> gyro_conversion{
        {_250, {0, 250}},
        {_500, {1, 500}},
        {_1000, {2, 1000}},
        {_2000, {3, 2000}}};
    std::map<ACCEL_FULL_SCALE_RANGE, g_scale_t> g_conversion{
        {_2, {0, 2}},
        {_4, {1, 4}},
        {_8, {2, 8}},
        {_16, {3, 16}}};

    void init_mpu();
    void convertData(MPUData_t *data, RawData_t *raw);
    uint8_t single_byte_write(uint8_t reg_addr, uint8_t reg_value);
    uint8_t single_byte_read(uint8_t reg_addr, uint8_t *dest);
    uint8_t burst_byte_read(uint8_t reg_addr, uint8_t *dest, uint8_t len);

public:
    MPU6050();
    void reset_enable_FIFO();
    void set_sample_rate(float rate);
    void set_gyro_resolution(GYRO_FULL_SCALE_RANGE range);
    void set_accel_resolution(ACCEL_FULL_SCALE_RANGE range);
    MPUData_t read_MPU_measure_from_registers();
    MPUData_t read_MPU_measure_from_FIFO();
    RawData_t read_registers_raw_data();
    RawData_t read_FIFO_raw_data();
    uint8_t read_interrupt_status();
    void config_FIFO();
    uint16_t read_FIFO_count();
};
