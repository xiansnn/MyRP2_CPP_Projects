add_executable(i2c_bus_scan
        bus_scan.c
        )

target_link_libraries(i2c_bus_scan pico_stdlib hardware_i2c)

pico_add_extra_outputs(i2c_bus_scan)
