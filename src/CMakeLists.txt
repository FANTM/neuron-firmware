
add_executable(neuron
        main.c
        mcp3464.c
        spi.c
        protocol.c
        usb_descriptors.c
        led_indicator.c
        )

pico_generate_pio_header(neuron ${CMAKE_CURRENT_LIST_DIR}/mcpclock.pio)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(neuron PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/../h)

# pull in common dependencies
target_link_libraries(neuron pico_stdlib pico_multicore hardware_spi hardware_uart hardware_pio tinyusb_device tinyusb_board)

# enable usb output, disable uart output
pico_enable_stdio_usb(neuron 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(neuron)
