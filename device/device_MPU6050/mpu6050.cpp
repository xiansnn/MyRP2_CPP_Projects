/**
 * @file mpu6050.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2023-05-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "mpu6050.h"

/**
 * @brief Construct a new MPU6050::MPU6050 object
 *
 */
MPU6050::MPU6050(i2c_inst_t *i2c, uint sda, uint scl, uint baud_rate)
{
    this->master = new hw_I2C_master(i2c, sda, scl, baud_rate);
    this->master->single_byte_write(MPU_ADDR, PWR_MGMT_1_RA, PWR_MGMT_1);
    // config FSYNC and DLPF config
    this->master->single_byte_write(MPU_ADDR, CONFIG_RA, CONFIG);
    uint8_t sample_div = int((GYRO_OUT_RATE / SAMPLE_RATE) - 1);
    this->master->single_byte_write(MPU_ADDR, SMPLRT_DIV_RA, sample_div);
    // set gyro sensor resolution
    this->gyro_factor = (float)GYRO_FULL_SCALE_RANGE / 32768.;
    this->master->single_byte_write(MPU_ADDR,GYRO_CONFIG_RA, GYRO_CONFIG);
    // set acceleration sensor resolution
    this->acceleration_factor = (float)ACCEL_FULL_SCALE_RANGE / 32768.;
    this->master->single_byte_write(MPU_ADDR, ACCEL_CONFIG_RA, ACCEL_CONFIG);
    // reset FIFO
    this->master->single_byte_write(MPU_ADDR, USER_CTRL_RA, FIFO_RESET);
    // enable FIFO
    this->master->single_byte_write(MPU_ADDR, USER_CTRL_RA, FIFO_EN);
    // configure sensors to write in FIFO
    this->master->single_byte_write(MPU_ADDR, FIFO_EN_RA, FIFO_SELECTED_SENSORS);
    // configure INT
    this->master->single_byte_write(MPU_ADDR, INT_PIN_CFG_RA, INT_PIN_CFG);
    // configure INT on Data ready
    this->master->single_byte_write(MPU_ADDR, INT_ENABLE_RA, INT_ENABLE);
    sleep_ms(1);
    this->calibrate();
}

void MPU6050::read_registers_all_raw_data(RawData_t *raw)
{
    uint8_t read_buf[14];
    this->master->burst_byte_read(MPU_ADDR,ACCEL_XOUT_H_RA, read_buf, 14);
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
    this->master->burst_byte_read(MPU_ADDR, FIFO_R_W_RA, read_buf, 14);
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
    this->master->burst_byte_read(MPU_ADDR, FIFO_R_W_RA, read_buf, 12);
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
    this->master->burst_byte_read(MPU_ADDR, FIFO_R_W_RA, read_buf, 6);
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
    this->master->burst_byte_read(MPU_ADDR, TEMP_OUT_H_RA, read_buf, 2);
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
    this->master->burst_byte_read(MPU_ADDR, FIFO_COUNT_H_RA, read_buf, 2);
    return (read_buf[0] << 8) + read_buf[1];
}

bool MPU6050::is_data_ready()
{
    uint8_t status[1];
    this->master->single_byte_read(MPU_ADDR,INT_STATUS_RA, status);

    return status[0] & DATA_RDY_INT;
}
