#include "sg90.h"
#include "mpu6050.h"

//-----------------------------------------------------------------------------
//                             MPU6050 I2C bus config
#define SENSOR_I2C_BUS i2c0
// #define I2C_BUS i2c1// set I2C speed Fast or Standard
#define SENSOR_I2C_SPEED I2C_STANDARD_MODE
// set I2C SDA and SCL pin
#define SENSOR_I2C_SDA 8 // gpio pin on MakerFab Primer Board
#define SENSOR_I2C_SCL 9 // gpio pin on MakerFab Primer Board
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//                             SG90 config
#define MOTOR_SYNC_PIN 6
#define MOTOR_COMMAND_PIN 7
#define MOTOR_POS_MIN -90
#define MOTOR_POS_MAX 90
//-----------------------------------------------------------------------------

int main(int argc, char const *argv[])
{
    SG90 motor = SG90(MOTOR_COMMAND_PIN, MOTOR_SYNC_PIN, MOTOR_POS_MIN, MOTOR_POS_MAX);
    MPU6050 sensor = MPU6050(SENSOR_I2C_BUS, SENSOR_I2C_SDA, SENSOR_I2C_SCL, SENSOR_I2C_SPEED);
    return 0;
}
