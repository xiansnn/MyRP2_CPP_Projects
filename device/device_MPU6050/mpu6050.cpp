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
#include "register_address_MPU6050.h"
#include "config_MPU6050.h"
#include "mpu6050.h"

/**
 * @brief Construct a new MPU6050::MPU6050 object
 *
 * @param master The associated I2C bus master
 * @param mpu_config
 */
MPU6050::MPU6050(hw_I2C_master *master, config_MPU6050_t mpu_config)
{
    this->master = master;
    this->config = mpu_config;
    this->init_mpu();
    sleep_ms(1);
    this->calibrate();
}

void MPU6050::read_registers_all_raw_data(RawData_t *raw)
{
    int nb;
    uint8_t read_buf[14];
    nb = this->master->burst_byte_read(this->config.MPU_ADDR, ACCEL_XOUT_H_RA, read_buf, 14);
    if (nb < 0 || nb < 14)
        this->init_mpu();

    this->raw.g_x = (read_buf[0] << 8) + read_buf[1];
    this->raw.g_y = (read_buf[2] << 8) + read_buf[3];
    this->raw.g_z = (read_buf[4] << 8) + read_buf[5];
    this->raw.temp_out = (read_buf[6] << 8) + read_buf[7];
    this->raw.gyro_x = (read_buf[8] << 8) + read_buf[9];
    this->raw.gyro_y = (read_buf[10] << 8) + read_buf[11];
    this->raw.gyro_z = (read_buf[12] << 8) + read_buf[13];

    raw->g_x = this->raw.g_x;
    raw->g_y = this->raw.g_y;
    raw->g_z = this->raw.g_z;
    raw->temp_out = this->raw.temp_out;
    raw->gyro_x = this->raw.gyro_x;
    raw->gyro_y = this->raw.gyro_y;
    raw->gyro_z = this->raw.gyro_z;
}

void MPU6050::read_FIFO_all_raw_data(RawData_t *raw)
{
    uint8_t read_buf[14];
    this->master->burst_byte_read(this->config.MPU_ADDR, FIFO_R_W_RA, read_buf, 14);
    raw->gyro_z = (read_buf[12] << 8) + read_buf[13];

    this->raw.g_x = (read_buf[0] << 8) + read_buf[1];
    this->raw.g_y = (read_buf[2] << 8) + read_buf[3];
    this->raw.g_z = (read_buf[4] << 8) + read_buf[5];
    this->raw.temp_out = (read_buf[6] << 8) + read_buf[7];
    this->raw.gyro_x = (read_buf[8] << 8) + read_buf[9];
    this->raw.gyro_y = (read_buf[10] << 8) + read_buf[11];
    this->raw.gyro_z = (read_buf[12] << 8) + read_buf[13];

    raw->g_x = this->raw.g_x;
    raw->g_y = this->raw.g_y;
    raw->g_z = this->raw.g_z;
    raw->temp_out = this->raw.temp_out;
    raw->gyro_x = this->raw.gyro_x;
    raw->gyro_y = this->raw.gyro_y;
    raw->gyro_z = this->raw.gyro_z;
}
void MPU6050::convert_raw_to_measure(RawData_t *raw, MPUData_t *data)
{
    this->data.g_x = this->raw.g_x * this->acceleration_factor + this->accel_x_offset;
    this->data.g_y = this->raw.g_y * this->acceleration_factor + this->accel_y_offset;
    this->data.g_z = this->raw.g_z * this->acceleration_factor + this->accel_z_offset;
    this->data.gyro_x = this->raw.gyro_x * this->gyro_factor + this->gyro_x_offset;
    this->data.gyro_y = this->raw.gyro_y * this->gyro_factor + this->gyro_y_offset;
    this->data.gyro_z = this->raw.gyro_z * this->gyro_factor + this->gyro_z_offset;
    this->data.temp_out = this->raw.temp_out * this->temperature_gain + this->temperature_offset;

    data->g_x = this->data.g_x;
    data->g_y = this->data.g_y;
    data->g_z = this->data.g_z;
    data->gyro_x = this->data.gyro_x;
    data->gyro_y = this->data.gyro_y;
    data->gyro_z = this->data.gyro_z;
    data->temp_out = this->data.temp_out;

}

void MPU6050::init_mpu()
{
    this->master->single_byte_write(this->config.MPU_ADDR, SIGNAL_PATH_RESET_RA, 0x07);

    this->master->single_byte_write(this->config.MPU_ADDR, PWR_MGMT_1_RA, this->config.PWR_MGMT_1);
    // config FSYNC and DLPF config
    if (this->config.DLPF_BW == 250)
        this->config.GYRO_OUT_RATE = 8000;
    else
        this->config.GYRO_OUT_RATE = 1000;
    uint8_t DLPF_CFG;
    if (this->config.DLPF_BW == 250)
        DLPF_CFG = DLPF_CFG_250HZ;
    else if (this->config.DLPF_BW == 200)
        DLPF_CFG = DLPF_CFG_200HZ;
    else if (this->config.DLPF_BW == 100)
        DLPF_CFG = DLPF_CFG_100HZ;
    else if (this->config.DLPF_BW == 50)
        DLPF_CFG = DLPF_CFG_50HZ;
    else if (this->config.DLPF_BW == 20)
        DLPF_CFG = DLPF_CFG_20HZ;
    else if (this->config.DLPF_BW == 10)
        DLPF_CFG = DLPF_CFG_10HZ;
    else if (this->config.DLPF_BW == 5)
        DLPF_CFG = DLPF_CFG_5HZ;
    this->master->single_byte_write(this->config.MPU_ADDR, CONFIG_RA, this->config.EXT_SYNC | DLPF_CFG);
    uint8_t sample_div = int((this->config.GYRO_OUT_RATE / this->config.SAMPLE_RATE) - 1);
    this->master->single_byte_write(this->config.MPU_ADDR, SMPLRT_DIV_RA, sample_div);
    // set gyro sensor resolution
    this->gyro_factor = (float)this->config.GYRO_FULL_SCALE_RANGE / 32768.;
    uint8_t GYRO_CONFIG = 0;
    if (this->config.GYRO_FULL_SCALE_RANGE == 250)
        GYRO_CONFIG = GYRO_RANGE_250DPS;
    else if (this->config.GYRO_FULL_SCALE_RANGE == 500)
        GYRO_CONFIG = GYRO_RANGE_500DPS;
    else if (this->config.GYRO_FULL_SCALE_RANGE == 1000)
        GYRO_CONFIG = GYRO_RANGE_1000DPS;
    else if (this->config.GYRO_FULL_SCALE_RANGE == 2000)
        GYRO_CONFIG = GYRO_RANGE_2000DPS;
    uint8_t ACCEL_CONFIG = 0;
    if (this->config.ACCEL_FULL_SCALE_RANGE == 2)
        ACCEL_CONFIG = ACCEL_RANGE_2G;
    else if (this->config.ACCEL_FULL_SCALE_RANGE == 4)
        ACCEL_CONFIG = ACCEL_RANGE_4G;
    else if (this->config.ACCEL_FULL_SCALE_RANGE == 8)
        ACCEL_CONFIG = ACCEL_RANGE_8G;
    else if (this->config.ACCEL_FULL_SCALE_RANGE == 16)
        ACCEL_CONFIG = ACCEL_RANGE_16G;
    this->master->single_byte_write(this->config.MPU_ADDR, GYRO_CONFIG_RA, GYRO_CONFIG);
    // set acceleration sensor resolution
    this->acceleration_factor = (float)this->config.ACCEL_FULL_SCALE_RANGE / 32768.;
    this->master->single_byte_write(this->config.MPU_ADDR, ACCEL_CONFIG_RA, ACCEL_CONFIG);
    // reset FIFO
    this->master->single_byte_write(this->config.MPU_ADDR, USER_CTRL_RA, FIFO_RESET);
    // enable FIFO operations
    this->master->single_byte_write(this->config.MPU_ADDR, USER_CTRL_RA, this->config.FIFO_OP);
    // configure sensors to write in FIFO
    this->master->single_byte_write(this->config.MPU_ADDR, FIFO_EN_RA, this->config.FIFO_SELECTED_SENSORS);
    // configure INT
    this->master->single_byte_write(this->config.MPU_ADDR, INT_PIN_CFG_RA, this->config.INT_PIN_CFG);
    // configure INT on Data ready
    this->master->single_byte_write(this->config.MPU_ADDR, INT_ENABLE_RA, this->config.INT_ENABLE);
}

void MPU6050::read_FIFO_g_accel_raw_data(RawData_t *raw)
{
    uint8_t read_buf[12];
    this->master->burst_byte_read(this->config.MPU_ADDR, FIFO_R_W_RA, read_buf, 12);
    
    this->raw.g_x = (read_buf[0] << 8) + read_buf[1];
    this->raw.g_y = (read_buf[2] << 8) + read_buf[3];
    this->raw.g_z = (read_buf[4] << 8) + read_buf[5];
    this->raw.gyro_x = (read_buf[6] << 8) + read_buf[7];
    this->raw.gyro_y = (read_buf[8] << 8) + read_buf[9];
    this->raw.gyro_z = (read_buf[10] << 8) + read_buf[11];

    raw->g_x = this->raw.g_x;
    raw->g_y = this->raw.g_y;
    raw->g_z = this->raw.g_z;
    raw->gyro_x = this->raw.gyro_x;
    raw->gyro_y = this->raw.gyro_y;
    raw->gyro_z = this->raw.gyro_z;

}
void MPU6050::read_FIFO_accel_raw_data(RawData_t *raw)
{
    uint8_t read_buf[6];
    this->master->burst_byte_read(this->config.MPU_ADDR, FIFO_R_W_RA, read_buf, 6);
    
    this->raw.g_x = (read_buf[0] << 8) + read_buf[1];
    this->raw.g_y = (read_buf[2] << 8) + read_buf[3];
    this->raw.g_z = (read_buf[4] << 8) + read_buf[5];

    raw->g_x = this->raw.g_x;
    raw->g_y = this->raw.g_y;
    raw->g_z = this->raw.g_z;

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
            accel_x += (float)this->raw.g_x;
            accel_y += (float)this->raw.g_y;
            accel_z += (float)this->raw.g_z;
            gyro_x += (float) this->raw.gyro_x;
            gyro_y += (float) this->raw.gyro_y;
            gyro_z += (float) this->raw.gyro_z;
            i++;
        }
        sleep_ms(1);
    }
    float ax = accel_x / nb_sample;
    float ay = accel_y / nb_sample;
    float az = accel_z / nb_sample;
    float g = 32768.0 / this->config.ACCEL_FULL_SCALE_RANGE;
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
    this->master->burst_byte_read(this->config.MPU_ADDR, TEMP_OUT_H_RA, read_buf, 2);
    // int16_t temp_out = (read_buf[0] << 8) + read_buf[1];
    this->raw.temp_out = (read_buf[0] << 8) + read_buf[1];
    return (float)this->raw.temp_out * this->temperature_gain + this->temperature_offset;
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
    this->master->burst_byte_read(this->config.MPU_ADDR, FIFO_COUNT_H_RA, read_buf, 2);
    return (read_buf[0] << 8) + read_buf[1];
}

bool MPU6050::is_data_ready()
{
    uint8_t status[1];
    this->master->single_byte_read(this->config.MPU_ADDR, INT_STATUS_RA, status);

    return status[0] & DATA_RDY_INT;
}
