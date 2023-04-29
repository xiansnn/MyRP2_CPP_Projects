add_library(hw_i2c hw_i2c.cpp)
target_link_libraries(hw_i2c pico_stdlib hardware_i2c pico_i2c_slave)

add_executable(i2c_bus_scan    bus_scan.c   )
target_link_libraries(i2c_bus_scan pico_stdlib hardware_i2c pico_i2c_slave)
pico_add_extra_outputs(i2c_bus_scan)

add_executable(slave_mem_i2c slave_mem_i2c.c )
target_link_libraries(slave_mem_i2c pico_i2c_slave hardware_i2c  pico_stdlib )
pico_add_extra_outputs(slave_mem_i2c)
