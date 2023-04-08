#include "include/mpu6050.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include <stdio.h>

#define I2C_SDA 8
#define I2C_SCL 9
#define MPU_ADDR 0x68
#define SAMPLE_RATE 100.0

int main()
{
    stdio_init_all();
    i2c_bus_data i2c_bus = {i2c0, I2C_SDA, I2C_SCL};
    MPU6050 mpu = MPU6050(i2c_bus, MPU_ADDR, SAMPLE_RATE);
    sleep_ms(100);
    while (true)
    {
        sleep_ms(1000);
        MPUData_t measures = mpu.readDataFromRegister();
        printf("Acc. X = %.2f, Y = %.2f, Z = %.2f", measures.g_x, measures.g_y, measures.g_z);
        printf("\tGyro. X = %.2f, Y = %.2f, Z = %.2f", measures.gyro_x, measures.gyro_y, measures.gyro_z);
        printf("\t\tTemp. = %.2f\n", measures.temp_out);

        sleep_ms(1000);
    }

    return 0;
}
