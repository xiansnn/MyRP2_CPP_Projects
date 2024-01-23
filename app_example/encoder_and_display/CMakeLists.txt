# add_executable(display_encoder encoder.cpp)
# target_include_directories(display_encoder PUBLIC 
#         ${PROJECT_SOURCE_DIR}/device/device_KY_040
#         ${PROJECT_SOURCE_DIR}/device/device_ssd1306
#         ${PROJECT_SOURCE_DIR}/utilities/dbg_probe)
# target_link_libraries(display_encoder PUBLIC  debug_probe ssd1306 ky_040)