add_executable(test_pwm
        test_pwm.cpp
        pwm_hal.cpp
        ../probe/probe.cpp
        )
target_link_libraries(test_pwm pico_stdlib hardware_pwm)
pico_add_extra_outputs(test_pwm)

add_executable(test_pwm_irq
        test_pwm_irq.cpp
        pwm_hal.cpp
        ../probe/probe.cpp
        )
target_link_libraries(test_pwm_irq pico_stdlib hardware_pwm)
pico_add_extra_outputs(test_pwm_irq)

add_executable(test_pwm_duty
        test_pwm_duty.cpp
        pwm_hal.cpp
        ../probe/probe.cpp
        )
target_link_libraries(test_pwm_duty pico_stdlib hardware_pwm)
pico_add_extra_outputs(test_pwm_duty)
