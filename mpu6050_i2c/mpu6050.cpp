#include "mpu6050.h"
#include "pico/stdlib.h"
#include "config_MPU6050.h"

MPU6050::MPU6050()
{
    this->sample_rate = SAMPLE_RATE;
    i2c_init(I2C_BUS, I2C_SPEED);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);
    gpio_pull_up(I2C_SDA);
    this->set_clock_src_DLPF_and_sample_rate(this->sample_rate, DLPF_BW::HZ_250);
    this->set_gyro_resolution(GYRO_FULL_SCALE_RANGE::DPS_250);
    this->set_accel_resolution(ACCEL_FULL_SCALE_RANGE::G_2);
    this->reset_enable_config_FIFO();
}

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

void MPU6050::set_clock_src_DLPF_and_sample_rate(float rate, DLPF_BW bw)
{
    // config clock source
    this->single_byte_write(PWR_MGMT_1_RA, PWR_MGMT_1_CLKSEL);
    // config FSYNC and DLPF config
    int gyro_out_rate{};
    uint8_t dlpf_code{};
    switch (bw)
    {
    case DLPF_BW::HZ_250:
        gyro_out_rate = 8000; // gyro output rate = 8000Hz when DLPF is disabled 1000Hz otherwise
        dlpf_code = 0x00;
        break;
    case DLPF_BW::HZ_200:
        gyro_out_rate = 1000;
        dlpf_code = 0x01;
        break;
    case DLPF_BW::HZ_100:
        gyro_out_rate = 1000;
        dlpf_code = 0x02;
        break;
    case DLPF_BW::HZ_50:
        gyro_out_rate = 1000;
        dlpf_code = 0x03;
        break;
    case DLPF_BW::HZ_20:
        gyro_out_rate = 1000;
        dlpf_code = 0x04;
        break;
    case DLPF_BW::HZ_10:
        gyro_out_rate = 1000;
        dlpf_code = 0x05;
        break;
    case DLPF_BW::HZ_5:
        gyro_out_rate = 1000;
        dlpf_code = 0x06;
    default:
        gyro_out_rate = 8000;
        dlpf_code = 0x00;
        break;
    }
    uint8_t ext_sync_set_code = 0x00 << 3; //  external FSYNC input disabled
    uint8_t config = ext_sync_set_code | dlpf_code;
    this->single_byte_write(CONFIG_RA, config);

    // sample rate = gyro output rate /(1+SMPLRT_DIV)
    uint8_t sample_div = int((gyro_out_rate / rate) - 1);
    this->sample_rate = gyro_out_rate / (1 + sample_div);
    this->single_byte_write(SMPLRT_DIV_RA, sample_div);
}

void MPU6050::set_gyro_resolution(GYRO_FULL_SCALE_RANGE range)
{
    uint8_t max_gyro_scale_code{};
    uint16_t max_gyro_scale_value{};
    switch (range)
    {
    case GYRO_FULL_SCALE_RANGE::DPS_250:
        max_gyro_scale_code = 0x0 << 3;
        max_gyro_scale_value = 250;
        break;
    case GYRO_FULL_SCALE_RANGE::DPS_500:
        max_gyro_scale_code = 0x1 << 3;
        max_gyro_scale_value = 500;
        break;
    case GYRO_FULL_SCALE_RANGE::DPS_1000:
        max_gyro_scale_code = 0x2 << 3;
        max_gyro_scale_value = 1000;
        break;
    case GYRO_FULL_SCALE_RANGE::DPS_2000:
        max_gyro_scale_code = 0x3 << 3;
        max_gyro_scale_value = 2000;
        break;
    }
    this->gyro_factor = (float)max_gyro_scale_value / 32768.;
    this->single_byte_write(GYRO_CONFIG_RA, max_gyro_scale_code);
}

void MPU6050::set_accel_resolution(ACCEL_FULL_SCALE_RANGE range)
{
    uint8_t max_g_scale_code{};
    uint8_t max_g_scale_value{};
    switch (range)
    {
    case ACCEL_FULL_SCALE_RANGE::G_2:
        max_g_scale_code = 0x00 << 3;
        max_g_scale_value = 2;
        break;
    case ACCEL_FULL_SCALE_RANGE::G_4:
        max_g_scale_code = 0x01 << 3;
        max_g_scale_value = 4;
        break;
    case ACCEL_FULL_SCALE_RANGE::G_8:
        max_g_scale_code = 0x02 << 3;
        max_g_scale_value = 8;
        break;
    case ACCEL_FULL_SCALE_RANGE::G_16:
        max_g_scale_code = 0x03 << 3;
        max_g_scale_value = 16;
        break;
    }

    this->acceleration_factor = (float)max_g_scale_value / 32768;
    this->single_byte_write(ACCEL_CONFIG_RA, max_g_scale_code);
}

void MPU6050::read_MPU_all_measure_from_registers(MPUData_t *data)
{
    RawData_t raw;
    this->read_registers_all_raw_data(&raw);
    data->g_x = raw.g_x * this->acceleration_factor;
    data->g_y = raw.g_y * this->acceleration_factor;
    data->g_z = raw.g_z * this->acceleration_factor;
    data->gyro_x = raw.gyro_x * this->gyro_factor;
    data->gyro_y = raw.gyro_y * this->gyro_factor;
    data->gyro_z = raw.gyro_z * this->gyro_factor;
    data->temp_out = raw.temp_out * this->temperature_gain + this->temperature_offset;
}

void MPU6050::read_registers_all_raw_data(RawData_t *measures)
{
    uint8_t read_buf[14];
    this->burst_byte_read(ACCEL_XOUT_H_RA, read_buf, 14);
    measures->g_x = (read_buf[0] << 8) + read_buf[1];
    measures->g_y = (read_buf[2] << 8) + read_buf[3];
    measures->g_z = (read_buf[4] << 8) + read_buf[5];
    measures->temp_out = (read_buf[6] << 8) + read_buf[7];
    measures->gyro_x = (read_buf[8] << 8) + read_buf[9];
    measures->gyro_y = (read_buf[10] << 8) + read_buf[11];
    measures->gyro_z = (read_buf[12] << 8) + read_buf[13];
}

void MPU6050::read_FIFO_all_raw_data(RawData_t *measures)
{
    uint8_t read_buf[14];
    this->burst_byte_read(FIFO_R_W_RA, read_buf, 14);
    measures->g_x = (read_buf[0] << 8) + read_buf[1];
    measures->g_y = (read_buf[2] << 8) + read_buf[3];
    measures->g_z = (read_buf[4] << 8) + read_buf[5];
    measures->temp_out = (read_buf[6] << 8) + read_buf[7];
    measures->gyro_x = (read_buf[8] << 8) + read_buf[9];
    measures->gyro_y = (read_buf[10] << 8) + read_buf[11];
    measures->gyro_z = (read_buf[12] << 8) + read_buf[13];
}

void MPU6050::read_FIFO_g_accel_raw_data(RawData_t *measures)
{
    uint8_t read_buf[12];
    this->burst_byte_read(FIFO_R_W_RA, read_buf, 12);
    measures->g_x = (read_buf[0] << 8) + read_buf[1];
    measures->g_y = (read_buf[2] << 8) + read_buf[3];
    measures->g_z = (read_buf[4] << 8) + read_buf[5];
    measures->gyro_x = (read_buf[6] << 8) + read_buf[7];
    measures->gyro_y = (read_buf[8] << 8) + read_buf[9];
    measures->gyro_z = (read_buf[10] << 8) + read_buf[11];
}

void MPU6050::reset_enable_config_FIFO()
{
    this->single_byte_write(USER_CTRL_RA, USER_CTRL_FIFO_RESET);
    sleep_us(250);
    this->single_byte_write(USER_CTRL_RA, USER_CTRL_FIFO_EN);
    sleep_us(250);
    this->single_byte_write(FIFO_EN_RA, FIFO_EN_VALUE); // write all MPU data into FIFO, assuming no external sensor
}

float MPU6050::read_MPU_temperature()
{
    uint8_t read_buf[2];
    this->burst_byte_read(TEMP_OUT_H_RA, read_buf, 2);
    int16_t temp_out = (read_buf[0] << 8) + read_buf[1];
    return (float)temp_out * this->temperature_gain + this->temperature_offset;
}

uint16_t MPU6050::read_FIFO_count()
{
    uint8_t read_buf[2] = {FIFO_COUNT_H_RA};
    this->burst_byte_read(FIFO_COUNT_H_RA, read_buf, 2);
    return (read_buf[0] << 8) + read_buf[1];
}

void MPU6050::read_MPU_all_measure_from_FIFO(MPUData_t *data)
{
    RawData_t raw;
    this->read_FIFO_all_raw_data(&raw);
    data->g_x = raw.g_x * this->acceleration_factor;
    data->g_y = raw.g_y * this->acceleration_factor;
    data->g_z = raw.g_z * this->acceleration_factor;
    data->gyro_x = raw.gyro_x * this->gyro_factor;
    data->gyro_y = raw.gyro_y * this->gyro_factor;
    data->gyro_z = raw.gyro_z * this->gyro_factor;
    data->temp_out = raw.temp_out * this->temperature_gain + this->temperature_offset;
}

void MPU6050::read_MPU_g_accel_measures_from_FIFO(MPUData_t *data)
{
    RawData_t raw{};
    this->read_FIFO_g_accel_raw_data(&raw);
    data->g_x = raw.g_x * this->acceleration_factor;
    data->g_y = raw.g_y * this->acceleration_factor;
    data->g_z = raw.g_z * this->acceleration_factor;
    data->gyro_x = raw.gyro_x * this->gyro_factor;
    data->gyro_y = raw.gyro_y * this->gyro_factor;
    data->gyro_z = raw.gyro_z * this->gyro_factor;
}

uint8_t MPU6050::read_interrupt_status()
{
    uint8_t read_buf;
    this->single_byte_read(INT_STATUS_RA, &read_buf);
    return read_buf;
}
