add_executable(hc_sr04
        hc_sr04.cpp
        test_hc_sr04.cpp
        )

# pull in common dependencies and additional i2c hardware support
# target_link_libraries(mpu6050_i2c pico_stdlib hardware_i2c)
target_link_libraries(hc_sr04 pico_stdlib)

# create map/bin/hex file etc.
# pico_add_extra_outputs(mpu6050_i2c)
pico_add_extra_outputs(hc_sr04)