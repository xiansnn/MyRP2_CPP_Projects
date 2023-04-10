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
    this->init_mpu();
    this->set_sample_rate(this->sample_rate);
    this->set_gyro_resolution(GYRO_FULL_SCALE_RANGE::_250);
    this->set_accel_resolution(ACCEL_FULL_SCALE_RANGE::_2);
    this->reset_enable_FIFO();
    this->config_FIFO();
}
void MPU6050::init_mpu()
{
    // config clock source
    this->single_byte_write(PWR_MGMT_1_RA, PWR_MGMT_1_CLKSEL );
    // config FSYNC and DLPF config
    this->single_byte_write(CONFIG_RA, CONFIG_FSYNC_DLPF );
    // WHO_AM_I
    uint8_t addr;
    this->single_byte_read(WHO_AM_I_RA,&addr);
}

uint8_t MPU6050::single_byte_write(uint8_t reg_addr, uint8_t reg_value)
{
    uint8_t write_buf[] = {reg_addr, reg_value};
    uint8_t nb = i2c_write_blocking(I2C_BUS, MPU_ADDR, write_buf, 2, false);
    return nb;
}

uint8_t MPU6050::single_byte_read(uint8_t reg_addr, uint8_t * dest)
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

void MPU6050::set_sample_rate(float rate)
{
    // gyro output rate = 8000Hz when DLPF is disabled
    // sample rate = gyro output rate /(1+SMPLRT_DIV)
    uint8_t sample_div = int((8000 / rate) - 1);
    this->sample_rate = GYRO_OUT_RATE / (1 + sample_div);
    this->single_byte_write(SMPLRT_DIV_RA, sample_div );
}

void MPU6050::set_gyro_resolution(GYRO_FULL_SCALE_RANGE range)
{
    gyro_scale_t range_map = this->gyro_conversion[range];
    uint8_t max_gyro_code = range_map.max_gyro_code <<3;
    uint16_t max_gyro_value = range_map.max_gyro_value;
    this->gyro_factor = (float)max_gyro_value / 32768.;
    this->single_byte_write(GYRO_CONFIG_RA, max_gyro_code );

}

void MPU6050::set_accel_resolution(ACCEL_FULL_SCALE_RANGE range)
{
    g_scale_t range_map = this->g_conversion[range];
    uint8_t g_scale_code = range_map.max_g_code <<3;
    uint8_t g_scale_value = range_map.max_g_value;
    this->acceleration_factor = (float)g_scale_value / 32768;
    this->single_byte_write(ACCEL_CONFIG_RA, g_scale_code);

}

MPUData_t MPU6050::read_MPU_measure_from_registers()
{
    RawData_t raw_data = this->read_registers_raw_data();
    MPUData_t data;
    this->convertData(&data, &raw_data);
    return data;
}

RawData_t MPU6050::read_registers_raw_data()
{
    uint8_t read_buf[14];
    RawData_t measures;
    this->burst_byte_read(ACCEL_XOUT_H_RA, read_buf, 14);
    measures.g_x = (read_buf[0] << 8) + read_buf[1];
    measures.g_y = (read_buf[2] << 8) + read_buf[3];
    measures.g_z = (read_buf[4] << 8) + read_buf[5];
    measures.temp_out = (read_buf[6] << 8) + read_buf[7];
    measures.gyro_x = (read_buf[8] << 8) + read_buf[9];
    measures.gyro_y = (read_buf[10] << 8) + read_buf[11];
    measures.gyro_z = (read_buf[12] << 8) + read_buf[13];
    return measures;
}

RawData_t MPU6050::read_FIFO_raw_data()
{
    uint8_t read_buf[14];
    RawData_t measures;
    this->burst_byte_read(FIFO_R_W_RA, read_buf, 14);
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

void MPU6050::reset_enable_FIFO()
{
    this->single_byte_write(USER_CTRL_RA, 0x0) ;// reset all bits
    this->single_byte_write(USER_CTRL_RA, USER_CTRL_FIFO_RESET) ;
    this->single_byte_write(USER_CTRL_RA, USER_CTRL_FIFO_EN) ;
}

void MPU6050::config_FIFO()
{
    this->single_byte_write(FIFO_EN_RA, FIFO_EN_VALUE);// write all MPU data into FIFO, assuming no external sensor
}

uint16_t MPU6050::read_FIFO_count()
{
    uint8_t read_buf[2] = {FIFO_COUNT_H_RA};
    this->burst_byte_read(FIFO_COUNT_H_RA,read_buf,2);
    return (read_buf[0]<<8) + read_buf[1];
}

MPUData MPU6050::read_MPU_measure_from_FIFO()
{
    RawData_t raw_data = this->read_FIFO_raw_data();
    MPUData_t data;
    this->convertData(&data, &raw_data);
    return data;
}

uint8_t MPU6050::read_interrupt_status()
{
    uint8_t read_buf;
    this->single_byte_read(INT_STATUS_RA, & read_buf);
    return read_buf;
}
