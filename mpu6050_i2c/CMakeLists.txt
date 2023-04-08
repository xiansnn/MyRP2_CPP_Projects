include_directories(include/*.h)


add_executable(mpu6050
        mpu6050.cpp
        test_mpu6050.cpp
        )

# pull in common dependencies and additional i2c hardware support
# target_link_libraries(mpu6050_i2c pico_stdlib hardware_i2c)
target_link_libraries(mpu6050 pico_stdlib hardware_i2c)

# create map/bin/hex file etc.
# pico_add_extra_outputs(mpu6050_i2c)
pico_add_extra_outputs(mpu6050)
