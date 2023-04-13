#include "config_MPU6050.h"
#include "mpu6050.h"
#include "pico/stdlib.h"

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
    this->gyro_factor = (float)GYRO_FULL_SCALE_VALUE / 32768.;
    this->single_byte_write(GYRO_CONFIG_RA, GYRO_CONFIG);
    // set acceleration sensor resolution
    this->acceleration_factor = (float)ACCEL_FULL_SCALE_VALUE / 32768.;
    this->single_byte_write(ACCEL_CONFIG_RA, ACCEL_CONFIG);
    // reset FIFO
    this->single_byte_write(USER_CTRL_RA, FIFO_RESET);
    sleep_us(250);
    // enable FIFO
    this->single_byte_write(USER_CTRL_RA, FIFO_EN);
    sleep_us(250);
    // configure sensors to write in FIFO
    this->single_byte_write(FIFO_EN_RA, FIFO_SELECTED_SENSOR);
    this->calibrate();
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

void MPU6050::calibrate()
{
    int64_t accel_x{};
    int64_t accel_y{};
    int64_t accel_z{};
    int64_t gyro_x{};
    int64_t gyro_y{};
    int64_t gyro_z{};
    int16_t nb_sample = 1024;

    for (int16_t i = 0; i < nb_sample; i++)
    {
        sleep_ms(1);
        RawData_t raw{};
        this->read_registers_all_raw_data(&raw);
        accel_x += raw.g_x;
        accel_y += raw.g_y;
        accel_z += raw.g_z;
        gyro_x += raw.gyro_x;
        gyro_y += raw.gyro_y;
        gyro_z += raw.gyro_z;
    }
    this->accel_x_offset = -(float)accel_x / nb_sample;
    this->accel_y_offset = -accel_y / nb_sample;
    this->accel_z_offset = 32768.0 / ACCEL_FULL_SCALE_RANGE - (float)accel_z / nb_sample;
    this->gyro_x_offset = -(float)gyro_x / nb_sample;
    this->gyro_y_offset = -(float)gyro_y / nb_sample;
    this->gyro_z_offset = -(float)gyro_z / nb_sample;
}
void MPU6050::read_MPU_all_measure_from_FIFO(MPUData_t *data)
{
    RawData_t raw;
    this->read_FIFO_all_raw_data(&raw);
    data->g_x = (raw.g_x + this->accel_x_offset) * this->acceleration_factor;
    data->g_y = (raw.g_y + this->accel_y_offset) * this->acceleration_factor;
    data->g_z = (raw.g_z + this->accel_z_offset) * this->acceleration_factor;
    data->gyro_x = (raw.gyro_x + this->gyro_x_offset) * this->gyro_factor;
    data->gyro_y = (raw.gyro_y + this->gyro_y_offset) * this->gyro_factor;
    data->gyro_z = (raw.gyro_z + this->gyro_z_offset) * this->gyro_factor;
    data->temp_out = raw.temp_out * this->temperature_gain + this->temperature_offset;
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
    data->g_x = (raw.g_x + this->accel_x_offset) * this->acceleration_factor;
    data->g_y = (raw.g_y + this->accel_y_offset) * this->acceleration_factor;
    data->g_z = (raw.g_z + this->accel_z_offset) * this->acceleration_factor;
    data->gyro_x = (raw.gyro_x + this->gyro_x_offset) * this->gyro_factor;
    data->gyro_y = (raw.gyro_y + this->gyro_y_offset) * this->gyro_factor;
    data->gyro_z = (raw.gyro_z + this->gyro_z_offset) * this->gyro_factor;
}

void MPU6050::read_MPU_all_measure_from_registers(MPUData_t *data)
{
    RawData_t raw;
    this->read_registers_all_raw_data(&raw);
    data->g_x = (raw.g_x + this->accel_x_offset) * this->acceleration_factor;
    data->g_y = (raw.g_y + this->accel_y_offset) * this->acceleration_factor;
    data->g_z = (raw.g_z + this->accel_z_offset) * this->acceleration_factor;
    data->gyro_x = (raw.gyro_x + this->gyro_x_offset) * this->gyro_factor;
    data->gyro_y = (raw.gyro_y + this->gyro_y_offset) * this->gyro_factor;
    data->gyro_z = (raw.gyro_z + this->gyro_z_offset) * this->gyro_factor;
    data->temp_out = raw.temp_out * this->temperature_gain + this->temperature_offset;
}

uint16_t MPU6050::read_FIFO_count()
{
    uint8_t read_buf[2] = {FIFO_COUNT_H_RA};
    this->burst_byte_read(FIFO_COUNT_H_RA, read_buf, 2);
    return (read_buf[0] << 8) + read_buf[1];
}

uint8_t MPU6050::read_interrupt_status()
{
    uint8_t read_buf;
    this->single_byte_read(INT_STATUS_RA, &read_buf);
    return read_buf;
}
