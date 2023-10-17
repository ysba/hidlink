cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(hidlink)

pico_sdk_init()

add_executable(hidlink
    main.c
    usb_descriptors.c
    hidlink_uart.c
    )

target_include_directories(hidlink PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(hidlink PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board)


#pico_enable_stdio_usb(hidlink 1)
pico_enable_stdio_uart(hidlink 0)
pico_add_extra_outputs(hidlink)