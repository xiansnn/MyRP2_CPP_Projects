include_directories(${PROJECT_SOURCE_DIR})

add_library(hc_sr04 hc_sr04.cpp)
target_link_libraries(hc_sr04 pico_stdlib)


add_executable(test_hc_sr04 test_hc_sr04.cpp )
target_link_libraries(test_hc_sr04 pico_stdlib hc_sr04)
pico_add_extra_outputs(test_hc_sr04)