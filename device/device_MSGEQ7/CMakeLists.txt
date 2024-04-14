add_library(msgeq7 msgeq7.cpp)
target_link_libraries(msgeq7 pico_stdlib hardware_adc)


add_executable(test_msgeq7 test_msgeq7.cpp)
target_link_libraries(test_msgeq7 pico_stdlib 
                                    msgeq7 
                                    debug_probe)
pico_add_extra_outputs(test_msgeq7)
