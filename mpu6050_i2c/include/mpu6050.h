// #include <stdio.h>
// #include <string.h>
#include <map>
#include "hardware/i2c.h"

#define I2C_SPEED 400000

typedef struct g_scale
{
    uint8_t g_range_code;
    uint8_t max_g_value;
} g_scale_t;
enum ACCEL_FULL_SCALE_RANGE
{
    _2 = 0,
    _4 = 1,
    _8 = 2,
    _16 = 3
};

typedef struct gyro_scale
{
    uint8_t gyro_scale_code;
    uint16_t gyro_scale_value;
} gyro_scale_t;
enum GYRO_FULL_SCALE_RANGE
{
    _250,
    _500,
    _1000,
    _2000
};

typedef struct i2c_bus_data
{
    i2c_inst_t *i2c;
    uint sda_gpio;
    uint scl_gpio;
} i2c_bus_data_t;

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

class MPU6050
{
private:
    uint8_t address;
    i2c_bus_data_t i2c_bus_data;
    float acceleration_factor;
    float gyro_factor;
    float temperature_gain = (float)1 / 340;
    float temperature_offset = 36.53;
    float sample_rate;
    std::map<GYRO_FULL_SCALE_RANGE, gyro_scale_t> gyro_conversion{
    {_250,  {0, 250}},
    {_500,  {1, 500}},
    {_1000, {2, 1000}},
    {_2000, {3, 2000}}};
    std::map<ACCEL_FULL_SCALE_RANGE, g_scale_t> g_conversion{
    {_2, {0, 2}},
    {_4, {1, 4}},
    {_8, {2, 4}},
    {_16,{3, 16}}};

    void init_mpu();
    void convertData(MPUData_t *data, RawData_t *raw);
    uint8_t read_MPU_register(uint8_t reg_addr);

public:
    MPU6050(i2c_bus_data_t i2c, uint8_t addr, float sample_rate);
    void setSampleRate(float rate);
    void setGyroResolution(GYRO_FULL_SCALE_RANGE range);
    void setGResolution(ACCEL_FULL_SCALE_RANGE range);
    MPUData_t readDataFromRegister();
    RawData_t readRawData();

    uint8_t readInterruptStatus();
    void enableFIFO();
    uint readFIFOcount();
    void resetFIFO();
    MPUData_t readDataFromFIFO();
};
