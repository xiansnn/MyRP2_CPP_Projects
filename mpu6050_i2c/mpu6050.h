// #include <stdio.h>
// #include <string.h>
#include <map>
#include "hardware/i2c.h"

// configuration of accelerometer
// typedef struct g_scale
// {
//     uint8_t max_g_code;
//     uint8_t max_g_value;
// } g_scale_t;
enum ACCEL_FULL_SCALE_RANGE
{
    G_2, // range +/- 2g
    G_4, // range +/- 4g
    G_8, // range +/- 8g
    G_16 // range +/- 16g
};

// configuration of gyrometer
// typedef struct gyro_scale
// {
//     uint8_t max_gyro_code;
//     uint16_t max_gyro_value;
// } gyro_scale_t;
enum GYRO_FULL_SCALE_RANGE
{
    DPS_250,  // range +/- 250 degrees per second
    DPS_500,  // range +/- 500 degrees per second
    DPS_1000, // range +/- 1000 degrees per second
    DPS_2000  // range +/- 2000 degrees per second
};

// configuration of the Digital Low-Pass Filter
enum DLPF_BW{     //            Accelero           |                       Gyro
    HZ_250=0,     // BW = 260Hz | delai =  0.0ms   |   BW = 256Hz | delai =  0.98ms | gyro_sampling_rate = 8kHz 
    HZ_200=1,     // BW = 184Hz | delai =  2.0ms   |   BW = 188Hz | delai =  1.90ms | gyro_sampling_rate = 1kHz 
    HZ_100=2,     // BW =  94Hz | delai =  3.0ms   |   BW =  98Hz | delai =  2.80ms | gyro_sampling_rate = 1kHz 
    HZ_50=3,      // BW =  44Hz | delai =  4.9ms   |   BW =  42Hz | delai =  4.80ms | gyro_sampling_rate = 1kHz 
    HZ_20=4,      // BW =  21Hz | delai =  8.5ms   |   BW =  20Hz | delai =  8.30ms | gyro_sampling_rate = 1kHz 
    HZ_10=5,      // BW =  10Hz | delai = 13.8ms   |   BW =  10Hz | delai = 13.40ms | gyro_sampling_rate = 1kHz 
    HZ_5=6        // BW =   5Hz | delai = 19.0ms   |   BW =   5Hz | delai = 18.60ms | gyro_sampling_rate = 1kHz 
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
    float acceleration_factor{};
    float gyro_factor{};
    float temperature_gain = (float)1 / 340;
    float temperature_offset = 36.53;
    float sample_rate{};

    uint8_t single_byte_write(uint8_t reg_addr, uint8_t reg_value);
    uint8_t single_byte_read(uint8_t reg_addr, uint8_t *dest);
    uint8_t burst_byte_read(uint8_t reg_addr, uint8_t *dest, uint8_t len);
    void set_clock_src_DLPF_and_sample_rate(float rate, DLPF_BW bandwidth);
    void set_gyro_resolution(GYRO_FULL_SCALE_RANGE range);
    void set_accel_resolution(ACCEL_FULL_SCALE_RANGE range);
    void read_registers_all_raw_data(RawData_t * raw);
    void read_FIFO_all_raw_data(RawData_t * raw);
    void read_FIFO_g_accel_raw_data(RawData_t * raw);

public:
    MPU6050();
    void reset_enable_config_FIFO();
    uint8_t read_interrupt_status();
    uint16_t read_FIFO_count();

    float read_MPU_temperature();
    void read_MPU_all_measure_from_registers(MPUData_t * data);
    void read_MPU_all_measure_from_FIFO(MPUData_t * data);
    void read_MPU_g_accel_measures_from_FIFO(MPUData_t * data);
};
