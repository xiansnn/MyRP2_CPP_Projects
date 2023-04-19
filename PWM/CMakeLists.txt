add_executable(pwm_example1
    example_pwm_1.cpp
    )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(pwm_example1 pico_stdlib hardware_pwm)
pico_add_extra_outputs(pwm_example1)

add_executable(pwm_example2
    example_pwm_2.cpp
    )
    target_link_libraries(pwm_example2 pico_stdlib hardware_pwm)
    pico_add_extra_outputs(pwm_example2)
    

add_executable(pwm_example3
    example_pwm_3.cpp
    )
    target_link_libraries(pwm_example3 pico_stdlib hardware_pwm)
    pico_add_extra_outputs(pwm_example3)
    
# create map/bin/hex file etc.
# pico_add_extra_outputs(mpu6050_i2c)
