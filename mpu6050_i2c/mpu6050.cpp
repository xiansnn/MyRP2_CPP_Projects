#include "include/mpu6050.h"
#include "pico/stdlib.h"

MPU6050::MPU6050(i2c_bus_data_t i2c_bus_data, uint8_t addr, float sample_rate)
{
    this->i2c_bus_data = i2c_bus_data;
    this->address = addr;
    this->sample_rate = sample_rate;
    i2c_init(this->i2c_bus_data.i2c, I2C_SPEED);
    gpio_set_function(this->i2c_bus_data.scl_gpio, GPIO_FUNC_I2C);
    gpio_set_function(this->i2c_bus_data.sda_gpio, GPIO_FUNC_I2C);
    gpio_pull_up(this->i2c_bus_data.sda_gpio);
    gpio_pull_up(this->i2c_bus_data.scl_gpio);
    this->init_mpu();
    this->setSampleRate(this->sample_rate);
    this->setGyroResolution(GYRO_FULL_SCALE_RANGE::_250);
    this->setGResolution(ACCEL_FULL_SCALE_RANGE::_2);
}
void MPU6050::init_mpu()
{
    uint8_t write_buf[2] = {};
    uint8_t read_buf[2] = {};

// hardware reset device
#define PWR_MGMT_1_RA 0x6B
    write_buf[0] = PWR_MGMT_1_RA;
// init clock source
#define PWR_MGMT_1_CLKSEL_INTERNAL 0x00
#define PWR_MGMT_1_CLKSEL_X_PLL 0x01
#define PWR_MGMT_1_CLKSEL_Y_PLL 0x02
#define PWR_MGMT_1_CLKSEL_Z_PLL 0x03
    write_buf[1] = PWR_MGMT_1_CLKSEL_Y_PLL;
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, write_buf, 2, false);
// set FSYNC and DLPF config
#define CONFIG_RA 0x1A
#define CONFIG_FSYNC_DISABLED 0x0
#define CONFIG_DLPF_DISABLED 0x0
    write_buf[0] = CONFIG_RA;
    write_buf[1] = CONFIG_FSYNC_DISABLED | CONFIG_DLPF_DISABLED;
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, write_buf, 2, false);
// WHO_AM_I
#define WHO_AM_I_RA 0x75
    read_buf[0] = WHO_AM_I_RA;
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, read_buf, 1, true);
    i2c_read_blocking(this->i2c_bus_data.i2c, this->address, read_buf, 1, false);
}

void MPU6050::setSampleRate(float rate)
{
// gyro output rate = 8000Hz when DLPF is disabled
// sample rate = gyro output rate /(1+SMPLRT_DIV)
#define SMPLRT_DIV_RA 0x19
#define GYRO_OUT_RATE 8000 // assuming DLPF is disabled,
    uint8_t write_buf[2] = {};
    uint8_t sample_div = int((8000 / rate) - 1);
    this->sample_rate = GYRO_OUT_RATE / (1 + sample_div);
    write_buf[0] = SMPLRT_DIV_RA;
    write_buf[1] = sample_div;
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, write_buf, 2, false);
}

void MPU6050::setGyroResolution(GYRO_FULL_SCALE_RANGE range)
{
#define GYRO_CONFIG_RA 0x1B
    gyro_scale_t range_map = this->gyro_conversion[range];
    uint8_t gyro_scale_code = range_map.gyro_scale_code;
    uint16_t gyro_scale_value = range_map.gyro_scale_value;
    this->gyro_factor = (float)gyro_scale_value / 32768.;
    uint8_t write_buf[2] = {};
    write_buf[0] = GYRO_CONFIG_RA;
    write_buf[1] = gyro_scale_code << 3;
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, write_buf, 2, false);
}

void MPU6050::setGResolution(ACCEL_FULL_SCALE_RANGE range)
{
#define ACCEL_CONFIG_RA 0x1C
    g_scale_t range_map = this->g_conversion[range];
    uint8_t g_scale_code = range_map.g_range_code;
    uint8_t g_scale_value = range_map.max_g_value;
    this->acceleration_factor = (float)g_scale_value / 32768;
    uint8_t write_buf[2] = {};
    write_buf[0] = ACCEL_CONFIG_RA;
    write_buf[1] = g_scale_code << 3;

    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, write_buf, 2, false);
}

uint8_t MPU6050::read_MPU_register(uint8_t reg_addr)
{
    uint8_t read_buf[] = {reg_addr};
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, read_buf, 1, true);
    i2c_read_blocking(this->i2c_bus_data.i2c, this->address, read_buf, 1, false);
    return read_buf[0];
}

MPUData_t MPU6050::readDataFromRegister()
{
    RawData_t raw_data = this->readRawData();
    MPUData_t data;
    this->convertData(& data, &raw_data);
    return data;
}

RawData_t MPU6050::readRawData()
{
#define ACCEL_XOUT_H 0x3B // the first data serves as a base for all MPU measures
    uint8_t read_buf[14] = {};
    RawData_t measures;
    read_buf[0] = ACCEL_XOUT_H;
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, read_buf, 1, true);
    i2c_read_blocking(this->i2c_bus_data.i2c, this->address, read_buf, 14, false);
    measures.g_x = (read_buf[0] << 8) + read_buf[1];
    measures.g_y = (read_buf[2] << 8) + read_buf[3];
    measures.g_z = (read_buf[4] << 8) + read_buf[5];
    measures.temp_out = (read_buf[6] << 8) + read_buf[7];
    measures.gyro_x = (read_buf[8] << 8) + read_buf[9];
    measures.gyro_y = (read_buf[10] << 8) + read_buf[11];
    measures.gyro_z = (read_buf[12] << 8) + read_buf[13];
    return measures;
}

void MPU6050::convertData(MPUData_t *data, RawData_t *raw)
{
    data->g_x = raw->g_x * this->acceleration_factor;
    data->g_y = raw->g_y * this->acceleration_factor;
    data->g_z = raw->g_z * this->acceleration_factor;
    data->gyro_x = raw->gyro_x * this->gyro_factor;
    data->gyro_y = raw->gyro_y * this->gyro_factor;
    data->gyro_z = raw->gyro_z * this->gyro_factor;
    data->temp_out = raw->temp_out * this->temperature_gain + this->temperature_offset;
}

void MPU6050::resetFIFO()
{
#define USER_CTRL_RA 0x6A
#define USER_CTRL_FIFO_EN 0x40
#define USER_CTRL_I2C_MST_EN 0x20
#define USER_CTRL_I2C_IF_DIS 0x10 // MPU6000 only
#define USER_CTRL_FIFO_RESET 0x04
#define USER_CTRL_I2C_MST_RESET 0x02
#define USER_CTRL_SIG_COND_RESET 0x01
    uint8_t write_buf[2] = {};
    write_buf[0] = USER_CTRL_RA;
    write_buf[1] = 0x0; // reset all bits
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, write_buf, 2, false);
    write_buf[1] = USER_CTRL_FIFO_RESET;
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, write_buf, 2, false);
    write_buf[1] = USER_CTRL_FIFO_EN;
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, write_buf, 2, false);
}

void MPU6050::enableFIFO()
{
#define FIFO_EN_RA 0x23
#define FIFO_EN_TEMP_FIFO_EN 0x80
#define FIFO_EN_XG_FIFO_EN 0x40
#define FIFO_EN_YG_FIFO_EN 0x20
#define FIFO_EN_ZG_FIFO_EN 0x10
#define FIFO_EN_ACCEL_FIFO_EN 0x08
#define FIFO_EN_SLV2_FIFO_EN 0x04
#define FIFO_EN_SLV1_FIFO_EN 0x02
#define FIFO_EN_SLV0_FIFO_EN 0x01
    uint8_t write_buf[2] = {};
    write_buf[0] = FIFO_EN_RA;
    write_buf[1] = 0xF8; // write all MPU data into FIFO, assuming no external sensor
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, write_buf, 2, false);
}

uint MPU6050::readFIFOcount()
{
    return uint();
}

MPUData MPU6050::readDataFromFIFO()
{
    return MPUData();
}

uint8_t MPU6050::readInterruptStatus()
{
#define INT_STATUS_RA 0x3A
    uint8_t read_buf[] = {INT_STATUS_RA};
    i2c_write_blocking(this->i2c_bus_data.i2c, this->address, read_buf, 1, true);
    i2c_read_blocking(this->i2c_bus_data.i2c, this->address, read_buf, 1, false);
    return read_buf[0];
}
