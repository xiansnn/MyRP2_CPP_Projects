#if !defined(MPU6050_H)
#define MPU6050_H

#include "config_MPU6050.h"
#include "pico/stdlib.h"
#include "hw_I2C/hw_i2c.h"

/**
 * @brief raw data as they are captured by sensor
 * 
 */
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

/**
 * @brief measured sensor value after scale correction
 * 
 */
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

/**
 * @brief MPU 6050 device
 * 
 */
class MPU6050
{
private:
    hw_I2C_master* master;
    float acceleration_factor{};
    float gyro_factor{};
    float temperature_gain = 1.0 / 340.0;
    float temperature_offset = 36.53;
    float accel_x_offset {};
    float accel_y_offset {};
    float accel_z_offset {};
    float gyro_x_offset {};
    float gyro_y_offset {};
    float gyro_z_offset {};

    void read_registers_all_raw_data(RawData_t * raw);
    void read_FIFO_all_raw_data(RawData_t * raw);
    void convert_raw_to_measure(RawData_t * raw, MPUData_t * measures);

public:
    MPU6050(i2c_inst_t *i2c, uint sda, uint scl, uint baud_rate);
    void read_FIFO_g_accel_raw_data(RawData_t * raw);
    void read_FIFO_accel_raw_data(RawData_t * raw);
    uint16_t read_FIFO_count();
    bool is_data_ready();

    float read_MPU_temperature();
    void read_MPU_all_measure_from_registers(MPUData_t * data);
    void read_MPU_all_measure_from_FIFO(MPUData_t * data);
    void read_MPU_g_accel_measures_from_FIFO(MPUData_t * data);
    void calibrate();
};

#endif // MPU6050_H