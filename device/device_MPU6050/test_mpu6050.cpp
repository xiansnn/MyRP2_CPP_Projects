#include "mpu6050.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include <stdio.h>
#include <math.h>

//-----------------------------------------------------------------------------
//                             I2C bus config
#define I2C_BUS i2c0 // set I2C channel 0 or 1, SDA and SCL pins
#define I2C_SDA 8 // gpio pin on MakerFab Primer Board
#define I2C_SCL 9 // gpio pin on MakerFab Primer Board
#define I2C_SPEED I2C_STANDARD_MODE // set I2C speed Fast or Standard
//-----------------------------------------------------------------------------
//                             MPU6050 config
//-----------------------------------------------------------------------------

config_MPU6050_t default_config{
    .SAMPLE_RATE = 50,
    .DLPF_BW = 20
};

void process_data_from_register(MPU6050 mpu){
    MPUData_t measures{};
    mpu.read_MPU_all_measure_from_registers(&measures);
    printf("reg. accX = %.2f\taccY = %.2f\taccZ = %.2f", measures.g_x, measures.g_y, measures.g_z);
    printf("\tvecteur G: %.2f", sqrt(pow(measures.g_x, 2) + pow(measures.g_y, 2) + pow(measures.g_z, 2)));
    printf("\t\tgyroX = %.2f\tgyroY = %.2f\tgyroZ = %.2f", measures.gyro_x, measures.gyro_y, measures.gyro_z);
    printf("\n\n");
};
void process_data_from_FIFO(MPU6050 mpu){
    MPUData_t measures{};
    mpu.read_MPU_g_accel_measures_from_FIFO(&measures);
    printf("fifo AccX = %.2f\tY = %.2f\tZ = %.2f", measures.g_x, measures.g_y, measures.g_z);
    printf("\tvecteur G: %.2f", sqrt(pow(measures.g_x, 2) + pow(measures.g_y, 2) + pow(measures.g_z, 2)));
    printf("\tGyroX = %.2f\tY = %.2f\tZ = %.2f", measures.gyro_x, measures.gyro_y, measures.gyro_z);
    printf("\n\n");
};

void process_raw_data_from_FIFO(MPU6050 mpu){
    RawData_t raw{};
    // mpu.read_FIFO_accel_raw_data(&raw);
    mpu.read_FIFO_g_accel_raw_data(&raw);
    printf(" FIFO Acc [X = %4x\t\tY = %4x\t\tZ = %4x ]", raw.g_x, raw.g_y, raw.g_z);
    printf("\t\t\tGyro [X = %4x\t\tY = %4x\t\tZ = %4x ]", raw.gyro_x, raw.gyro_y, raw.gyro_z);
    printf("\n\n");
};

int main()
{
    stdio_init_all();

    // create I2C bus hw peripheral
    hw_I2C_master master = hw_I2C_master(I2C_BUS, I2C_SDA, I2C_SCL, I2C_SPEED);
    MPU6050 mpu = MPU6050( &master , default_config  );

    // MPU6050 mpu = MPU6050(I2C_BUS, I2C_SDA, I2C_SCL, I2C_SPEED);
    float t = mpu.read_MPU_temperature();
    printf("temperature : %.2f\n",t);
    while (true)
    {
        if (mpu.is_data_ready())
        {
            // process_raw_data_from_FIFO(mpu);
            process_data_from_register(mpu); 
            // process_data_from_FIFO(mpu);    
        }
        sleep_ms(1);
    }
    return 0;
}
