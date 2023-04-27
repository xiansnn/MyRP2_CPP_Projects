/**
 * @file mpu6050.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "config_MPU6050.h"
#include "mpu6050.h"
#include "hardware/i2c.h"

/**
 * @brief Construct a new MPU6050::MPU6050 object
 * 
 */
MPU6050::MPU6050()
{
    // config I2C
    i2c_init(I2C_BUS, I2C_SPEED);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);
    gpio_pull_up(I2C_SDA);
    // config clock source
    this->single_byte_write(PWR_MGMT_1_RA, PWR_MGMT_1);
    // config FSYNC and DLPF config
    this->single_byte_write(CONFIG_RA, CONFIG);
    // sample rate = gyro output rate /(1+SMPLRT_DIV)
    uint8_t sample_div = int((GYRO_OUT_RATE / SAMPLE_RATE) - 1);
    // this->sample_rate = GYRO_OUT_RATE / (1 + sample_div);
    this->single_byte_write(SMPLRT_DIV_RA, sample_div);
    // set gyro sensor resolution
    this->gyro_factor = (float)GYRO_FULL_SCALE_RANGE / 32768.;
    this->single_byte_write(GYRO_CONFIG_RA, GYRO_CONFIG);
    // set acceleration sensor resolution
    this->acceleration_factor = (float)ACCEL_FULL_SCALE_RANGE / 32768.;
    this->single_byte_write(ACCEL_CONFIG_RA, ACCEL_CONFIG);
    // reset FIFO
    this->single_byte_write(USER_CTRL_RA, FIFO_RESET);
    // enable FIFO
    this->single_byte_write(USER_CTRL_RA, FIFO_EN);
    // configure sensors to write in FIFO
    this->single_byte_write(FIFO_EN_RA, FIFO_SELECTED_SENSORS);
    // configure INT
    this->single_byte_write(INT_PIN_CFG_RA, INT_PIN_CFG);
    // configure INT on Data ready
    this->single_byte_write(INT_ENABLE_RA, INT_ENABLE);
    this->calibrate();
}

/**
 * @brief 
 * 
 * @param reg_addr 
 * @param reg_value 
 * @return uint8_t 
 */
uint8_t MPU6050::single_byte_write(uint8_t reg_addr, uint8_t reg_value)
{
    uint8_t write_buf[] = {reg_addr, reg_value};
    uint8_t nb = i2c_write_blocking(I2C_BUS, MPU_ADDR, write_buf, 2, false);
    return nb;
}

uint8_t MPU6050::single_byte_read(uint8_t reg_addr, uint8_t *dest)
{
    uint8_t read_buf[]{reg_addr};
    i2c_write_blocking(I2C_BUS, MPU_ADDR, read_buf, 1, true);
    uint8_t nb = i2c_read_blocking(I2C_BUS, MPU_ADDR, dest, 1, false);
    return nb;
}

uint8_t MPU6050::burst_byte_read(uint8_t reg_addr, uint8_t *dest, uint8_t len)
{
    i2c_write_blocking(I2C_BUS, MPU_ADDR, &reg_addr, 1, true);
    uint8_t nb = i2c_read_blocking(I2C_BUS, MPU_ADDR, dest, len, false);
    return nb;
}
void MPU6050::read_registers_all_raw_data(RawData_t *raw)
{
    uint8_t read_buf[14];
    this->burst_byte_read(ACCEL_XOUT_H_RA, read_buf, 14);
    raw->g_x = (read_buf[0] << 8) + read_buf[1];
    raw->g_y = (read_buf[2] << 8) + read_buf[3];
    raw->g_z = (read_buf[4] << 8) + read_buf[5];
    raw->temp_out = (read_buf[6] << 8) + read_buf[7];
    raw->gyro_x = (read_buf[8] << 8) + read_buf[9];
    raw->gyro_y = (read_buf[10] << 8) + read_buf[11];
    raw->gyro_z = (read_buf[12] << 8) + read_buf[13];
}

void MPU6050::read_FIFO_all_raw_data(RawData_t *raw)
{
    uint8_t read_buf[14];
    this->burst_byte_read(FIFO_R_W_RA, read_buf, 14);
    raw->g_x = (read_buf[0] << 8) + read_buf[1];
    raw->g_y = (read_buf[2] << 8) + read_buf[3];
    raw->g_z = (read_buf[4] << 8) + read_buf[5];
    raw->temp_out = (read_buf[6] << 8) + read_buf[7];
    raw->gyro_x = (read_buf[8] << 8) + read_buf[9];
    raw->gyro_y = (read_buf[10] << 8) + read_buf[11];
    raw->gyro_z = (read_buf[12] << 8) + read_buf[13];
}
void MPU6050::convert_raw_to_measure(RawData_t *raw, MPUData_t *data)
{
    data->g_x = raw->g_x * this->acceleration_factor + this->accel_x_offset;
    data->g_y = raw->g_y * this->acceleration_factor + this->accel_y_offset;
    data->g_z = raw->g_z * this->acceleration_factor + this->accel_z_offset;
    data->gyro_x = raw->gyro_x * this->gyro_factor + this->gyro_x_offset;
    data->gyro_y = raw->gyro_y * this->gyro_factor + this->gyro_y_offset;
    data->gyro_z = raw->gyro_z * this->gyro_factor + this->gyro_z_offset;
    data->temp_out = raw->temp_out * this->temperature_gain + this->temperature_offset;
}

void MPU6050::read_FIFO_g_accel_raw_data(RawData_t *raw)
{
    uint8_t read_buf[12];
    this->burst_byte_read(FIFO_R_W_RA, read_buf, 12);
    raw->g_x = (read_buf[0] << 8) + read_buf[1];
    raw->g_y = (read_buf[2] << 8) + read_buf[3];
    raw->g_z = (read_buf[4] << 8) + read_buf[5];
    raw->gyro_x = (read_buf[6] << 8) + read_buf[7];
    raw->gyro_y = (read_buf[8] << 8) + read_buf[9];
    raw->gyro_z = (read_buf[10] << 8) + read_buf[11];
}
void MPU6050::read_FIFO_accel_raw_data(RawData_t *raw)
{
    uint8_t read_buf[6];
    this->burst_byte_read(FIFO_R_W_RA, read_buf, 6);
    raw->g_x = (read_buf[0] << 8) + read_buf[1];
    raw->g_y = (read_buf[2] << 8) + read_buf[3];
    raw->g_z = (read_buf[4] << 8) + read_buf[5];
}

void MPU6050::calibrate()
{
    float accel_x{};
    float accel_y{};
    float accel_z{};
    float gyro_x{};
    float gyro_y{};
    float gyro_z{};
    size_t nb_sample{100};

    size_t i = 0;
    while (i < nb_sample)
    {
        if (this->is_data_ready())
        {
            RawData_t raw{};
            this->read_registers_all_raw_data(&raw);
            accel_x += (float)raw.g_x;
            accel_y += (float)raw.g_y;
            accel_z += (float)raw.g_z;
            gyro_x += (float)raw.gyro_x;
            gyro_y += (float)raw.gyro_y;
            gyro_z += (float)raw.gyro_z;
            i++;
        }
        sleep_ms(1);
    }
    float ax = accel_x / nb_sample;
    float ay = accel_y / nb_sample;
    float az = accel_z / nb_sample;
    float g = 32768.0 / ACCEL_FULL_SCALE_RANGE;
    float gx = gyro_x / nb_sample;
    float gy = gyro_y / nb_sample;
    float gz = gyro_z / nb_sample;

    this->accel_x_offset = (-ax) * this->acceleration_factor;
    this->accel_y_offset = (-ay) * this->acceleration_factor;
    this->accel_z_offset = (g - az) * this->acceleration_factor;
    this->gyro_x_offset = (-gx) * this->gyro_factor;
    this->gyro_y_offset = (-gy) * this->gyro_factor;
    this->gyro_z_offset = (-gz) * this->gyro_factor;
}
void MPU6050::read_MPU_all_measure_from_FIFO(MPUData_t *data)
{
    RawData_t raw;
    this->read_FIFO_all_raw_data(&raw);
    this->convert_raw_to_measure(&raw, data);
}

float MPU6050::read_MPU_temperature()
{
    uint8_t read_buf[2];
    this->burst_byte_read(TEMP_OUT_H_RA, read_buf, 2);
    int16_t temp_out = (read_buf[0] << 8) + read_buf[1];
    return (float)temp_out * this->temperature_gain + this->temperature_offset;
}
void MPU6050::read_MPU_g_accel_measures_from_FIFO(MPUData_t *data)
{
    RawData_t raw{};
    this->read_FIFO_g_accel_raw_data(&raw);
    this->convert_raw_to_measure(&raw, data);
}

void MPU6050::read_MPU_all_measure_from_registers(MPUData_t *data)
{
    RawData_t raw;
    this->read_registers_all_raw_data(&raw);
    this->convert_raw_to_measure(&raw, data);
}

uint16_t MPU6050::read_FIFO_count()
{
    uint8_t read_buf[2] = {FIFO_COUNT_H_RA};
    this->burst_byte_read(FIFO_COUNT_H_RA, read_buf, 2);
    return (read_buf[0] << 8) + read_buf[1];
}

bool MPU6050::is_data_ready()
{
    uint8_t status;
    this->single_byte_read(INT_STATUS_RA, &status);
    return status & DATA_RDY_INT;
}
