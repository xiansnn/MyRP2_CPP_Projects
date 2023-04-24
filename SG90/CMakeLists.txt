add_executable(test_sg90
    test_sg90.cpp
    ../PWM/pwm_hal.cpp
    )

target_link_libraries(test_sg90 pico_stdlib hardware_pwm)

pico_add_extra_outputs(test_sg90)
