#include "sg90.h"
#include "mpu6050.h"
#include <stdio.h>
#include "probe.h"

//-----------------------------------------------------------------------------
//                             MPU6050 I2C bus and sensor device_config
config_master_i2c_t i2c_bus_cfg{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_STANDARD_MODE};

config_MPU6050_t sensor_config{
    .SAMPLE_RATE = 50,
    .DLPF_BW = 5,
    // .FIFO_SELECTED_SENSORS = GYRO_FIFO_EN | ACCEL_FIFO_EN,
    // .FIFO_OP = FIFO_EN
    .FIFO_SELECTED_SENSORS = FIFO_NO_SELECTION,
    .FIFO_OP = FIFO_DISABLE};
//-----------------------------------------------------------------------------
//                             SG90 device_config
config_sg90_t motor_config{
    .command_pin = 7,
    .sync_pin = 6,
    .pos_min_degree = -90,
    .pos_max_degree = +90};
//-----------------------------------------------------------------------------

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

int main()
{
    stdio_init_all();
    hw_I2C_master bus_master = hw_I2C_master(i2c_bus_cfg);
    SG90 motor = SG90(motor_config);
    MPU6050 sensor = MPU6050(&bus_master, sensor_config);
    motor.set_pos((motor_config.pos_max_degree + motor_config.pos_min_degree) / 2); // set pos at mid range
    sleep_ms(5000);
    

    while (true)
    {
        if (sensor.is_data_ready())
        {
            float dev;
            pr_D4.hi();
            MPUData_t measures = sensor.get_measures();

            pr_D4.lo(); 
            dev =    measures.g_z;
            // printf("dev : %+000.1f \n", dev);
            int pos = motor_config.pos_max_degree * dev;
            pr_D5.hi();
            motor.set_pos(pos);
            pr_D5.lo();
        }
        sleep_ms(5);
    }

    return 0;
}
