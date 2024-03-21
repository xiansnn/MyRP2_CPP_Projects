add_executable(display_bargraph display_bargraph.cpp)
target_link_libraries(display_bargraph 
                            PUBLIC  
                                widget_bargraph 
                                widget_simple_bargraph
                                msgeq7 
                                hw_i2c 
                                hardware_i2c 
                                debug_probe 
                                pico_stdlib 
                                ssd1306 
                                ui_mvc 
                                rotary_encoder 
                                widget_bar )
target_include_directories(display_bargraph PUBLIC 
        ${PROJECT_SOURCE_DIR}/device/device_MSGEQ7)


pico_add_extra_outputs(display_bargraph)