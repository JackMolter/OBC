cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(OBC)
add_library( obcnew INTERFACE)

pico_sdk_init()

target_sources(obcnew INTERFACE
${CMAKE_CURRENT_LIST_DIR}/main.c
${CMAKE_CURRENT_LIST_DIR}/barometer.c
${CMAKE_CURRENT_LIST_DIR}/lwgps.c
${CMAKE_CURRENT_LIST_DIR}/IMU.c
${CMAKE_CURRENT_LIST_DIR}/GPS.c
${CMAKE_CURRENT_LIST_DIR}/flash.c
${CMAKE_CURRENT_LIST_DIR}/OpenLST.c
${CMAKE_CURRENT_LIST_DIR}/LED.c
${CMAKE_CURRENT_LIST_DIR}/commands.c
)

target_link_libraries(obcnew INTERFACE
    pico_stdlib
    hardware_i2c
    hardware_uart
    hardware_spi
    hardware_irq
    hardware_dma
    hardware_pwm
)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} obcnew)
target_compile_definitions(${PROJECT_NAME} PRIVATE obcnew)

pico_enable_stdio_usb(${PROJECT_NAME} 1) # from video
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})