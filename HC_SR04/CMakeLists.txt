add_executable(hc_sr04
        hc_sr04.cpp
        test_hc_sr04.cpp
        )
target_link_libraries(hc_sr04 pico_stdlib)
pico_add_extra_outputs(hc_sr04)