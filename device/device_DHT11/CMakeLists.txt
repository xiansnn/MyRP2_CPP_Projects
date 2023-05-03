add_library(dht11 dht11.cpp)
target_link_libraries(dht11 pico_stdlib)

add_executable(test_dht11  test_dht.cpp  )   
target_link_libraries(test_dht11 pico_stdlib dht11)
pico_add_extra_outputs(test_dht11)

